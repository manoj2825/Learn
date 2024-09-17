// ============================================================================
// inikey.h
//
//     Copyright 2004 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2004.06.07
// ============================================================================

#ifndef _INIKEY_H
#define _INIKEY_H

// ================================================
// _/_/_/  XXXXXXX.mcf
// ================================================
// --- instllation
#define MCF_SEC_INSTLLATION							 L"Installation"

#define MCF_KEY_INST_DMDRIVERVERSION				 L"Version"
#define MCF_KEY_INST_MODELNAME						 L"ModelName"
#define MCF_KEY_INST_DRVVER							 L"DriverVersion"
#define MCF_KEY_INST_DRVVER_9X						 L"DriverVersion9x"
#define MCF_KEY_INST_DRVVER_NT						 L"DriverVersionNT"
#define MCF_KEY_INST_DRVVER_2K						 L"DriverVersion2K"
#define MCF_KEY_INST_DRVVER_XP						 L"DriverVersionXP"
#define MCF_KEY_INST_DRVEXT							 L"DriverExtention"
#define MCF_KEY_INST_DRVBUILDDATE					 L"DriverBuildDate"

// --- DefaultPrinter
#define MCF_SEC_DEFAULTPRINTER						 L"DefaultPrinter"
#define MCF_SEC_DEFAULTPRINTER_UPDATE				 L"DefaultPrinterUpdate"

// +++ PUBDMODE
#define MCF_KEY_DEF_P_DMDEVICENAME					 L"pub_dmDeviceName"
#define MCF_KEY_DEF_P_DMSPECVERSION					 L"pub_dmSpecVersion"
#define MCF_KEY_DEF_P_DMDRIVERVERSION				 L"pub_dmDriverVersion"
#define MCF_KEY_DEF_P_DMSIZE						 L"pub_dmSize"
#define MCF_KEY_DEF_P_DMDRIVEREXTRA					 L"pub_dmDriverExtra"
#define MCF_KEY_DEF_P_DMFIELDS						 L"pub_dmFields"
#define MCF_KEY_DEF_P_DMORIENTATION					 L"pub_dmOrientation"
#define MCF_KEY_DEF_P_DMPAPERSIZE					 L"pub_dmPaperSize"
#define MCF_KEY_DEF_P_DMPAPERLENGTH					 L"pub_dmPaperLength"
#define MCF_KEY_DEF_P_DMPAPERWIDTH					 L"pub_dmPaperWidth"
#define MCF_KEY_DEF_P_DMSCALE						 L"pub_dmScale"
#define MCF_KEY_DEF_P_DMCOPIES						 L"pub_dmCopies"
#define MCF_KEY_DEF_P_DMDEFAULTSOURCE				 L"pub_dmDefaultSource"
#define MCF_KEY_DEF_P_DMPRINTQUALITY				 L"pub_dmPrintQuality"
#define MCF_KEY_DEF_P_DMCOLOR						 L"pub_dmColor"
#define MCF_KEY_DEF_P_DMDUPLEX						 L"pub_dmDuplex"
#define MCF_KEY_DEF_P_DMYRESOLUTION					 L"pub_dmYResolution"
#define MCF_KEY_DEF_P_DMTTOPTION					 L"pub_dmTTOption"
#define MCF_KEY_DEF_P_DMCOLLATE						 L"pub_dmCollate"
#define MCF_KEY_DEF_P_DMFORMNAME					 L"pub_dmFormName"
#define MCF_KEY_DEF_P_DMLOGPIXELS					 L"pub_dmLogPixels"
#define MCF_KEY_DEF_P_DMBITSPERPEL					 L"pub_dmBitsPerPel"
#define MCF_KEY_DEF_P_DMPELSWIDTH					 L"pub_dmPelsWidth"
#define MCF_KEY_DEF_P_DMPELSHEIGHT					 L"pub_dmPelsHeight"
#define MCF_KEY_DEF_P_DMDISPLAYFLAGS				 L"pub_dmDisplayFlags"
#define MCF_KEY_DEF_P_DMDISPLAYFREQUENCY			 L"pub_dmDisplayFrequency"
#define MCF_KEY_DEF_P_DMICMMETHOD					 L"pub_dmICMMethod"
#define MCF_KEY_DEF_P_DMICMINTENT					 L"pub_dmICMIntent"
#define MCF_KEY_DEF_P_DMMEDIATYPE					 L"pub_dmMediaType"
#define MCF_KEY_DEF_P_DMDITHERTYPE					 L"pub_dmDitherType"

// +++ _DOCDMODE(dwFieldDDM)
#define MCF_KEY_DEF_D_DMRASTER						 L"doc_dm_raster"
#define MCF_KEY_DEF_D_NEGATIVEIMG					 L"doc_dm_negativeimg"
#define MCF_KEY_DEF_D_EUROCHARA						 L"doc_dm_eurochara"
#define MCF_KEY_DEF_D_PSERRORINFO					 L"doc_dm_pserrorinfo"
#define MCF_KEY_DEF_D_PSPASSTHROUGH					 L"doc_dm_pspassthrough"
#define MCF_KEY_DEF_D_RUSSURF_MONO					 L"doc_dm_russurf_mono"
#define MCF_KEY_DEF_D_RUSSURF_GRSC					 L"doc_dm_russurf_grsc"
#define MCF_KEY_DEF_D_RUSSURF_COLOR					 L"doc_dm_russurf_color"
#define MCF_KEY_DEF_D_RUSSURF_AUTOCHG				 L"doc_dm_russurf_autochg"
//#define MCF_KEY_DEF_D_JPEG_BASELINE				 L"doc_dm_jpeg_baseline"
#define MCF_KEY_DEF_D_FILLTOBLACK					 L"doc_dm_filltoblack"
#define MCF_KEY_DEF_D_FONT_SELECT					 L"doc_dm_font_select"
#define MCF_KEY_DEF_D_ALLTEXTBLACK					 L"doc_dm_alltextblack"
#define MCF_KEY_DEF_D_ALLVECTORBLACK				 L"doc_dm_allvectorblack"
#define MCF_KEY_DEF_D_USE_PMFBYPASS					 L"doc_dm_use_pmfbypass"
#define MCF_KEY_DEF_D_SCALE_MYSELF					 L"doc_dm_scale_myself"
#define MCF_KEY_DEF_D_PCLCOLOR						 L"doc_dm_pclcolor"
#define MCF_KEY_DEF_D_EMFJOURNAL					 L"doc_dm_emfjournal"
#define MCF_KEY_DEF_D_GRAPHMODE_AUTO				 L"doc_dm_graphmode_auto"
#define MCF_KEY_DEF_D_WYSIWYG						 L"doc_dm_wysiwyg"
#define MCF_KEY_DEF_D_LINEWIDTH_HW					 L"doc_dm_linewidht_hw"
#define MCF_KEY_DEF_D_COLORADJ_CORE					 L"doc_dm_coloradj_core"
#define MCF_KEY_DEF_D_PCFAX_MODE					 L"doc_dm_pcfax_mode"
#define MCF_KEY_DEF_D_SKIP_DEVENT					 L"doc_dm_skip_event"
#define MCF_KEY_DEF_D_JPEG_PASSTHROUGH				 L"doc_dm_jpeg_passthrough"
// >> 2007.11.14 SSL:K.Hamaguchi
// for CR2
#define MCF_KEY_DEF_D_LINEWIDTH_DOT					 L"doc_dm_llinewidth_dot"
// >> 2007.11.14 SSL:K.Hamaguchi
// >> 2007.11.01 SBC:Kawabata
// for CR2
#define MCF_KEY_DEF_D_SJIS_MSFONT					 L"doc_dm_sjis_msfont"
// << 2007.11.01 SBC:Kawabata
// >> 2007.12.04 SSL:K.Hamaguchi
// for CR2
#define MCF_KEY_DEF_D_ZOOM_CENTER					 L"doc_dm_zoom_center"
// << 2007.12.04 SSL:K.Hamaguchi
// >> 2008.03.05 SSL:K.Hamaguchi
// for CR2
#define MCF_KEY_DEF_D_COLORPRINT_ALERT				 L"doc_dm_colorprint_alert"
// << 2008.03.05 SSL:K.Hamaguchi

// +++ _DOCDMODE(members)
#define MCF_KEY_DEF_D_WSTRUCTSIZE					 L"doc_wStructSize"
#define MCF_KEY_DEF_D_BYJOBCOMP						 L"doc_byJobComp"
#define MCF_KEY_DEF_D_BYBITMAPCOMP					 L"doc_byBitmapComp"
#define MCF_KEY_DEF_D_BYBITMAPCOMPOPTION			 L"doc_byBitmapCompOption"
#define MCF_KEY_DEF_D_BYJPEGSAMPLEFACTOR			 L"doc_byJpegSampleFactor"
#define MCF_KEY_DEF_D_BYDLFORMAT					 L"doc_byDLFormat"
#define MCF_KEY_DEF_D_BYOUTPUTBPP					 L"doc_byOutputBPP"
#define MCF_KEY_DEF_D_BYPATTERNFILLRATIO			 L"doc_byUserPatternFillRatio"
#define MCF_KEY_DEF_D_BYOUTPUTBITMAP				 L"doc_byOutputBitmap"
#define MCF_KEY_DEF_D_BYOUTPUTTEXTS					 L"doc_byOutputTexts"
#define MCF_KEY_DEF_D_BYOUTPUTVECTORS				 L"doc_byOutputVectors"
#define MCF_KEY_DEF_D_BYMINPENWIDTH					 L"doc_byMinPenWidth"
// >> 2008.02.08 SBC:Kawabata
// for CR2
#if SCDM_VER >= 1918
#define MCF_KEY_DEF_D_BYREDUCED						 L"doc_byReduced"
#endif
// << 2008.02.08 SBC:Kawabata
#define MCF_KEY_DEF_D_DWIMAGEABLEWIDTHPIXEL			 L"doc_dwImageableWidthPixel"
#define MCF_KEY_DEF_D_DWIMAGEABLEHEIGHTPIXEL		 L"doc_dwImageableHeightPixel"
#define MCF_KEY_DEF_D_NLEFTMARGINPIXEL				 L"doc_nLeftMarginPixel"
#define MCF_KEY_DEF_D_NTOPMARGINPIXEL				 L"doc_nTopMarginPixel"
#define MCF_KEY_DEF_D_NSRCLEFTMARGINMILLI			 L"doc_nSrcLeftMarginMilli"
#define MCF_KEY_DEF_D_NSRCTOPMARGINMILLI			 L"doc_nSrcTopMarginMilli"
#define MCF_KEY_DEF_D_NSRCRIGHTMARGINMILLI			 L"doc_nSrcRightMarginMilli"
#define MCF_KEY_DEF_D_NSRCBOTTOMMARGINMILLI			 L"doc_nSrcBottomMarginMilli"
#define MCF_KEY_DEF_D_WPENWIDTHRATE					 L"doc_wPenWidthRate"
#define MCF_KEY_DEF_D_DWDOWNLOADRAM					 L"doc_dwDownloadRAM"
#define MCF_KEY_DEF_D_DWPAPERFOLD					 L"doc_dwPaperFold"
#define MCF_KEY_DEF_D_NINSCHAPTER					 L"doc_nInsChapter"
#define MCF_KEY_DEF_D_SZCOLORPROFILE				 L"doc_szColorProfile"
// >> 2010.08.06 SSL:K.Hamaguchi
// for Aries
#define MCF_KEY_DEF_D_BYTONERSAVECMB				 L"doc_dm_byTonerSaveCmb"
// << 2010.08.06 SSL:K.Hamaguchi

// >> 2008.04.23 SBC:Kawabata
// +++ _EXTDMODE(wFieldEDM)
#define MCF_KEY_DEF_E_DMWIDENING					 L"ext_dm_widening"
// << 2008.04.23 SBC:Kawabata
// >> 2008.08.19 SBC:Kawabata
// for C-Jupiter2 長尺印刷
#define MCF_KEY_DEF_E_DMLONGPRINT					 L"ext_dm_longprint"
// << 2008.08.19 SBC:Kawabata
// >> 2009.11.18 SSL:K.Hamaguchi
// for Dragon3
#define MCF_KEY_DEF_E_DMPRINTPOSITION				 L"ext_dm_printposition"
// << 2009.11.18 SSL:K.Hamaguchi

// >> 2010.01.07 SSL:K.Hamaguchi
// for Dragon3
#define MCF_KEY_DEF_E_DMPRINTPOSITION_CHG			 L"ext_dm_printposition_chg"
// << 2010.01.07 SSL:K.Hamaguchi

// >> 2010.04.12 SSL:K.Hamaguchi
#define MCF_KEY_DEF_E_DMPRINT_SPD_PRI				 L"ext_dm_printingspdpri"
// << 2010.04.12 SSL:K.Hamaguchi

// >> 2010.07.30 SSL:K.Hamaguchi
#define MCF_KEY_DEF_E_DMREGISTRATION				 L"ext_dm_registration"
#define MCF_KEY_DEF_E_DMLAYOUTPRINT					 L"ext_dm_layoutprint"
#define MCF_KEY_DEF_E_DMWOVEN						 L"ext_dm_woven"
#define MCF_KEY_DEF_E_DMSAMESZNUP					 L"ext_dm_samesizenup"
#define MCF_KEY_DEF_E_DMIMAGESEND					 L"ext_dm_imagesend"
#define MCF_KEY_DEF_E_DMOPENPDF						 L"ext_dm_openpdf"
#define MCF_KEY_DEF_E_DMRIPRGB						 L"ext_dm_ripstyle_rgb"
// << 2010.07.30 SSL:K.Hamaguchi

// >> 2011.01.14 SSL:K.Hamaguchi
#define MCF_KEY_DEF_E_DMA3WPAMPHLET					 L"ext_dm_a3w_pamphlet"
// << 2011.01.14 SSL:K.Hamaguchi

// +++ _EXTDMODE(dwFieldEDM1)
#define MCF_KEY_DEF_E_DMPUNCH						 L"ext_dm_punch"
#define MCF_KEY_DEF_E_DMDIFPAPER					 L"ext_dm_difpaper"
#define MCF_KEY_DEF_E_DMDIF2SIDED					 L"ext_dm_dif2sided"
#define MCF_KEY_DEF_E_DMTPINSERTS					 L"ext_dm_tpinserts"
#define MCF_KEY_DEF_E_DMTPPRINTED					 L"ext_dm_tpprinted"
#define MCF_KEY_DEF_E_DMRIGHTTRNPAMPH				 L"ext_dm_righttrnpamph"
#define MCF_KEY_DEF_E_DMPAGEPROTECT					 L"ext_dm_pageprotect"
#define MCF_KEY_DEF_E_DMROTATE180DEG				 L"ext_dm_rotate180deg"
#define MCF_KEY_DEF_E_DMOFFSET						 L"ext_dm_offset"
#define MCF_KEY_DEF_E_DMLONGEDGEFEED				 L"ext_dm_longedgefeed"
#define MCF_KEY_DEF_E_DMTANDEMPRINT					 L"ext_dm_tandemprint"
//#define MCF_KEY_DEF_E_DMALLPAPERTYPES				 L"ext_dm_allpapertypes"
// >> 2009.11.17 SBC:S.Kawabata
// for Dragon3
#define MCF_KEY_DEF_E_DMPAGEINTERLEAVE				 L"ext_dm_pageinterleave"
// << 2009.11.17 SBC:S.Kawabata
#define MCF_KEY_DEF_E_DMNUPBORDER					 L"ext_dm_nupborder"
#define MCF_KEY_DEF_E_DMPOSBORDER					 L"ext_dm_posborder"
#define MCF_KEY_DEF_E_DMFITTOPAGE					 L"ext_dm_fittopage"
#define MCF_KEY_DEF_E_DMTPTEXT						 L"ext_dm_tptext"
#define MCF_KEY_DEF_E_DMFIRSTPAGE					 L"ext_dm_firstpage"
#define MCF_KEY_DEF_E_DMOUTLINEONLY					 L"ext_dm_outlineonly"
#define MCF_KEY_DEF_E_DMQUERYOVERLAY				 L"ext_dm_queryoverlay"
#define MCF_KEY_DEF_E_DMUSEOVERLAY					 L"ext_dm_useoverlay"
#define MCF_KEY_DEF_E_DMCREATEOVERLAY				 L"ext_dm_createoverlay"
#define MCF_KEY_DEF_E_DMREVORDER					 L"ext_dm_revorder"
#define MCF_KEY_DEF_E_DMFORCEFITTOPAGE				 L"ext_dm_forcefittopage"
#define MCF_KEY_DEF_E_DMLOCKASPECTRATIO				 L"ext_dm_lockaspectratio"
#define MCF_KEY_DEF_E_DMXYFITTOPAGE					 L"ext_dm_xyfittopage"
#define MCF_KEY_DEF_E_DMXYZOOM						 L"ext_dm_xyzoom"
#define MCF_KEY_DEF_E_DMDIFCOVERBLANK				 L"ext_dm_difcoverblank"
#define MCF_KEY_DEF_E_DMDIFLASTBLANK				 L"ext_dm_diflastblank"
#define MCF_KEY_DEF_E_DMTABPRINT					 L"ext_dm_tabprint"
#define MCF_KEY_DEF_E_DMDIF1SIDED					 L"ext_dm_dif1sided"
#define MCF_KEY_DEF_E_DMEXTRESUOL					 L"ext_dm_ext_resolution"
#define MCF_KEY_DEF_E_DMREVERSEPRINT				 L"ext_dm_ext_reverseprint"

// +++ _EXTDMODE(dwFieldEDM2)
#define MCF_KEY_DEF_E_DMCARBONCOPY					 L"ext_dm_carboncopy"
#define MCF_KEY_DEF_E_DMNUPREPEAT					 L"ext_dm_nuprepeat"
#define MCF_KEY_DEF_E_DMROTATE180DEGHW				 L"ext_dm_rotate180deg_hw"
#define MCF_KEY_DEF_E_DMMARGINSHIFTHW				 L"ext_dm_marginshift_hw"
#define MCF_KEY_DEF_E_DMPAMPHLETHW					 L"ext_dm_pamphlet_hw"
#define MCF_KEY_DEF_E_DMREVORDERHW					 L"ext_dm_revorder_hw"
#define MCF_KEY_DEF_E_DMDUPLEXHW					 L"ext_dm_duplex_hw"
#define MCF_KEY_DEF_E_DMCOLLATEHW					 L"ext_dm_collate_hw"
#define MCF_KEY_DEF_E_DMUNCOLLATEHW					 L"ext_dm_uncollate_hw"
#define MCF_KEY_DEF_E_DMSMOOTHING					 L"ext_dm_smoothing"
#define MCF_KEY_DEF_E_DMPHOTOENHANCE				 L"ext_dm_photoenhance"
#define MCF_KEY_DEF_E_DMTONERSAVE					 L"ext_dm_tonersave"
#define MCF_KEY_DEF_E_DMCUSTOMTA					 L"ext_dm_customta"
#define MCF_KEY_DEF_E_DMMIRRORVERT					 L"ext_dm_mirrorvert"
#define MCF_KEY_DEF_E_DMMIRRORHORZ					 L"ext_dm_mirrorhorz"
#define MCF_KEY_DEF_E_DMCARBONCOPYHW				 L"ext_dm_carboncopy_hw"
#define MCF_KEY_DEF_E_DMEDGETOEDGE					 L"ext_dm_edgetoedge"
#define MCF_KEY_DEF_E_DMMGNSHIFTL					 L"ext_dm_mgnshiftl"
#define MCF_KEY_DEF_E_DMMGNSHIFTT					 L"ext_dm_mgnshiftt"
#define MCF_KEY_DEF_E_DMMGNSHIFTR					 L"ext_dm_mgnshiftr"
#define MCF_KEY_DEF_E_DMMGNSHIFTB					 L"ext_dm_mgnshiftb"
#define MCF_KEY_DEF_E_DMDOCUMENTCONTROL				 L"ext_dm_documentcontrol"
#define MCF_KEY_DEF_E_DMSPECCOLORMODE				 L"ext_dm_speccolormode"
#define MCF_KEY_DEF_E_DMWMTRANSROP					 L"ext_dm_wmtransrop"
#define MCF_KEY_DEF_E_DMINSCHAP						 L"ext_dm_inschap"
// >> 2007.11.22 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_DEF_E_DMPRICOLLATE					 L"ext_dm_pri_collate"
#define MCF_KEY_DEF_E_DMPRIDUPLEX					 L"ext_dm_pri_duplex"
#define MCF_KEY_DEF_E_DMPRICOLOR					 L"ext_dm_pri_color"
// << 2007.11.22 SSL:K.Hamaguchi

// >> 2010.08.17 SBC:Hashida
#define MCF_KEY_DEF_E_LPNUP                          L"ext_bylpnup"
#define MCF_KEY_DEF_E_LPTYPE                         L"ext_bylptype"
// << 2010.08.17 SBC:Hashida
// >> 2008.01.10 SBC:Kawabata
// >> for CR2
#define MCF_KEY_DEF_E_SHORTJOBNAME					 L"ext_dm_short_jobname"
// << 2008.01.10 SBC:Kawabata

// +++ _EXTDMODE(members)
#define MCF_KEY_DEF_E_WSTRUCTSIZE					 L"ext_wStructSize"
#define MCF_KEY_DEF_E_BYNUPX						 L"ext_byNupX"
#define MCF_KEY_DEF_E_BYNUPY						 L"ext_byNupY"
#define MCF_KEY_DEF_E_BYNUPORDER					 L"ext_byNupOrder"
#define MCF_KEY_DEF_E_BYPOSTER						 L"ext_byPoster"
#define MCF_KEY_DEF_E_BYPAMPH						 L"ext_byPamph"
#define MCF_KEY_DEF_E_BYBINDEDGE					 L"ext_byBindedge"
#define MCF_KEY_DEF_E_BYSTAPLE						 L"ext_byStaple"
#define MCF_KEY_DEF_E_BYCUSUNITS					 L"ext_byCusUnits"
#define MCF_KEY_DEF_E_BYZOOMUNIT					 L"ext_byZoomUnit"
#define MCF_KEY_DEF_E_BYMGNSHIFT					 L"ext_byMgnShift"
#define MCF_KEY_DEF_E_BYMGNSHIFTY					 L"ext_byMgnShiftY"
#define MCF_KEY_DEF_E_BYTHRESHOLDARRAY				 L"ext_byThresholdArray"
#define MCF_KEY_DEF_E_NZOOM							 L"ext_nZoom"
#define MCF_KEY_DEF_E_NZOOMY						 L"ext_nZoomY"
#define MCF_KEY_DEF_E_NZOOMSRCPAPERSIZE				 L"ext_nZoomSrcPaperSize"
#define MCF_KEY_DEF_E_NOVERLAP						 L"ext_nOverlap"
#define MCF_KEY_DEF_E_NFTPPAPERSIZE					 L"ext_nFTPPaperSize"
#define MCF_KEY_DEF_E_NPAMPHPAPERSIZE				 L"ext_nPamphPaperSize"
#define MCF_KEY_DEF_E_NSIGNATURE					 L"ext_nSignature"
#define MCF_KEY_DEF_E_NOUTPUT						 L"ext_nOutput"
#define MCF_KEY_DEF_E_NMGNSHIFTPIXEL				 L"ext_nMgnShiftPixel"
#define MCF_KEY_DEF_E_NMGNSHIFTYPIXEL				 L"ext_nMgnShiftYPixel"
#define MCF_KEY_DEF_E_NCBNTOPPAPERSOURCE			 L"ext_nCbnTopPaperSource"
#define MCF_KEY_DEF_E_NCBNPAPERSOURCE				 L"ext_nCbnPaperSource"
#define MCF_KEY_DEF_E_NDIFFPAPERSOURCE				 L"ext_nDiffPaperSource"
#define MCF_KEY_DEF_E_NDIFFPAPERTYPE				 L"ext_nDiffPaperType"
#define MCF_KEY_DEF_E_NDIFFPAPERSOURCELAST			 L"ext_nDiffPaperSourceLast"
#define MCF_KEY_DEF_E_NDIFFPAPERTYPELAST			 L"ext_nDiffPaperTypeLast"
#define MCF_KEY_DEF_E_NTRANSPAPERSOURCE				 L"ext_nTransPaperSource"
#define MCF_KEY_DEF_E_NTRANSPAPERTYPE				 L"ext_nTransPaperType"
#define MCF_KEY_DEF_E_NTABSHIFTPIXEL				 L"ext_nTabShiftPixel"
#define MCF_KEY_DEF_E_NOVERLAYFILE					 L"ext_nOverlayFile"
#define MCF_KEY_DEF_E_NWATERMARK					 L"ext_nWatermark"
#define MCF_KEY_DEF_E_NIMAGESTAMP					 L"ext_nImageStamp"
#define MCF_KEY_DEF_E_NANTICOPY						 L"ext_nAntiCopy"
#define MCF_KEY_DEF_E_NDIFFPAPER					 L"ext_nDiffPaper"
#define MCF_KEY_DEF_E_NTABPRINT						 L"ext_nTabPrint"
#define MCF_KEY_DEF_E_NCARBONCOPY					 L"ext_nCarbonCopy"
#define MCF_KEY_DEF_E_NICCSETTING					 L"ext_nICCSetting"
#define MCF_KEY_DEF_E_NDSTWIDTHMILLI				 L"ext_nDstWidthMilli"
#define MCF_KEY_DEF_E_NDSTHEIGHTMILLI				 L"ext_nDstHeightMilli"
#define MCF_KEY_DEF_E_NDSTLEFTMARGINPIXEL			 L"ext_nDstLeftMarginPixel"
#define MCF_KEY_DEF_E_NDSTTOPMARGINPIXEL			 L"ext_nDstTopMarginPixel"
#define MCF_KEY_DEF_E_DMDUPLEXSAVE					 L"ext_dmDuplexSave"
#define MCF_KEY_DEF_E_DMDEFAULTSOURCESAVE			 L"ext_dmDefaultSourceSave"
#define MCF_KEY_DEF_E_DWCUSWIDTHPIXEL				 L"ext_dwCusWidthPixel"
#define MCF_KEY_DEF_E_DWCUSLENGTHPIXEL				 L"ext_dwCusLengthPixel"
#define MCF_KEY_DEF_E_DWDSTWIDTHPIXEL				 L"ext_dwDstWidthPixel"
#define MCF_KEY_DEF_E_DWDSTHEIGHTPIXEL				 L"ext_dwDstHeightPixel"
#define MCF_KEY_DEF_E_DWZOOMWIDTHPIXEL				 L"ext_dwZoomWidthPixel"
#define MCF_KEY_DEF_E_DWZOOMHEIGHTPIXEL				 L"ext_dwZoomHeightPixel"
#define MCF_KEY_DEF_E_BYRESOLUTION					 L"ext_byResolution"
#define MCF_KEY_DEF_E_BYPOSTERSAVE					 L"ext_byPosterSave"
#define MCF_KEY_DEF_E_BYDOCPATTERNCOL				 L"ext_byDocPatternCol"
#define MCF_KEY_DEF_E_BYPUNCHTYPE					 L"ext_byPunchType"
#define MCF_KEY_DEF_E_NCBNTOPPAPERTYPE				 L"ext_nCbnTopPaperType"
#define MCF_KEY_DEF_E_NCBNPAPERTYPE					 L"ext_nCbnPaperType"
// >> 2010.05.11 SSL:K.Hamaguchi
#define MCF_KEY_DEF_E_NGETOUTPUTLEVEL				 L"ext_nGetOutputLevel"
// << 2010.05.11 SSL:K.Hamaguchi

// +++ _EXTDMODE(JobCtrl)
#define MCF_KEY_DEF_E_JC_WFIELDJC					 L"ext_jc_wFieldJC"
#define MCF_KEY_DEF_E_JC_BYRETENTION				 L"ext_jc_byRetention"
#define MCF_KEY_DEF_E_JC_BYPINENCRYPTION			 L"ext_jc_byPinEncryption"
#define MCF_KEY_DEF_E_JC_BYDOCUMENTFILING			 L"ext_jc_byDocumentFiling"
#define MCF_KEY_DEF_E_JC_NDOCFILINGFOLDER			 L"ext_jc_nDocFilingFolder"

// +++ _EXTDMODE(ColData)
#define MCF_KEY_DEF_E_CD_WFIELDCD					 L"ext_cd_wFieldCD"
#define MCF_KEY_DEF_E_CD_BYDOCTYPE					 L"ext_cd_byDocType"
#define MCF_KEY_DEF_E_CD_BYCOLRENDER				 L"ext_cd_byColRender"
#define MCF_KEY_DEF_E_CD_BYUCRSELECT				 L"ext_cd_byUCRSelect"
#define MCF_KEY_DEF_E_CD_BYSCREENINGBMP				 L"ext_cd_byScreeningBmp"
#define MCF_KEY_DEF_E_CD_BYSCREENINGGRAPH			 L"ext_cd_byScreeningGraph"
#define MCF_KEY_DEF_E_CD_BYSCREENINGTEXT			 L"ext_cd_byScreeningText"
#define MCF_KEY_DEF_E_CD_BYCOLMODE					 L"ext_cd_byColMode"
#define MCF_KEY_DEF_E_CD_BYPRNMODE					 L"ext_cd_byPrnMode"
#define MCF_KEY_DEF_E_CD_BYSPOTCOLORMATHING			 L"ext_cd_bySpotColorMathing"
#define MCF_KEY_DEF_E_CD_BYINPUTPROFILE				 L"ext_cd_byInputProfile"
#define MCF_KEY_DEF_E_CD_BYOUTPUTPROFILE			 L"ext_cd_byOutputProfile"
#define MCF_KEY_DEF_E_CD_BYINKSIMULATION			 L"ext_cd_byInkSimulation"
#define MCF_KEY_DEF_E_CD_BYINTENTBMP				 L"ext_cd_byIntentBmp"
#define MCF_KEY_DEF_E_CD_BYINTENTGRAPH				 L"ext_cd_byIntentGraph"
#define MCF_KEY_DEF_E_CD_BYINTENTTEXT				 L"ext_cd_byIntentText"
// >> 2008.01.10 SBC:Kawabata
// for CR2
#define MCF_KEY_DEF_E_CD_BYRESOLTYPE				 L"ext_cd_byResolType"
// << 2008.01.10 SBC:Kawabata
#define MCF_KEY_DEF_E_CD_NBRIGHTNESS				 L"ext_cd_nBrightness"
#define MCF_KEY_DEF_E_CD_NCONTRAST					 L"ext_cd_nContrast"
#define MCF_KEY_DEF_E_CD_NSATURATION				 L"ext_cd_nSaturation"
#define MCF_KEY_DEF_E_CD_NREDSTRENGTH				 L"ext_cd_nRedStrength"
#define MCF_KEY_DEF_E_CD_NGREENSTRENGTH				 L"ext_cd_nGreenStrength"
#define MCF_KEY_DEF_E_CD_NBLUESTRENGTH				 L"ext_cd_nBlueStrength"
#define MCF_KEY_DEF_E_CD_NGBRED						 L"ext_cd_nGBRed"
#define MCF_KEY_DEF_E_CD_NGBGREEN					 L"ext_cd_nGBGreen"
#define MCF_KEY_DEF_E_CD_NGBBLUE					 L"ext_cd_nGBBlue"
#define MCF_KEY_DEF_E_CD_NSCREENFREQ				 L"ext_cd_nScreenFreq"
#define MCF_KEY_DEF_E_CD_NSCREENANG					 L"ext_cd_nScreenAng"
#define MCF_KEY_DEF_E_CD_BYINPUTPROFILEDEVICE		 L"ext_cd_byInputProfileDevice"
#define MCF_KEY_DEF_E_CD_BYOUTPUTPROFILEDEVICE		 L"ext_cd_byOutputProfileDevice"
// >> 2008.05.26 SSL:K.Hamaguchi
// for Pegasus3 Light
#define MCF_KEY_DEF_E_CD_BYDENSITY					 L"ext_cd_byDensity"
// << 2008.05.26 SSL:K.Hamaguchi
// >> 2009.05.28 SSL:K.Hamaguchi
// for Andromeda2 Refresh
#define MCF_KEY_DEF_E_CD_BYDITHERID					 L"ext_cd_byDitherID"
// << 2008.05.26 SSL:K.Hamaguchi

// +++ _EXTDMODE(PCFax)
#define MCF_KEY_DEF_E_FX_WCOLORMODE					 L"ext_fx_wColorMode"
#define MCF_KEY_DEF_E_FX_WCOMPTYPE					 L"ext_fx_wCompType"
#define MCF_KEY_DEF_E_FX_WROTATE					 L"ext_fx_wRotate"

// +++ _DEVDMODE(dwFieldVDM)
#define MCF_KEY_DEF_V_DMBYPASSTRAY					 L"dev_dm_bypasstray"
#define MCF_KEY_DEF_V_DMLCC							 L"dev_dm_lcc"
#define MCF_KEY_DEF_V_DMSADDLESTITCH				 L"dev_dm_saddlestitch"
#define MCF_KEY_DEF_V_DMDUPLEXMODULE				 L"dev_dm_duplexmodule"
#define MCF_KEY_DEF_V_DMINVERTERUNIT				 L"dev_dm_inverterunit"
#define MCF_KEY_DEF_V_DMHARDDISK					 L"dev_dm_harddisk"
#define MCF_KEY_DEF_V_DMSCANNERUNIT					 L"dev_dm_scannerunit"
#define MCF_KEY_DEF_V_DMROPM						 L"dev_dm_ropm"
#define MCF_KEY_DEF_V_DMDATASECURITYKIT				 L"dev_dm_datasecuritykit"
#define MCF_KEY_DEF_V_DMINSERTERUNIT				 L"dev_dm_inserterunit"
#define MCF_KEY_DEF_V_DMGBCPUNCH					 L"dev_dm_gbcpunch"
#define MCF_KEY_DEF_V_DMFOLDINGUNIT					 L"dev_dm_foldingunit"
// >> 2009.11.17 SSL:K.Hamaguchi
#define MCF_KEY_DEF_V_PP_USERAUTH					 L"dev_dm_userauth"
#define MCF_KEY_DEF_V_PP_RETENTION					 L"dev_dm_retention"
// << 2009.11.17 SSL:K.Hamaguchi
//#define MCF_KEY_DEF_V_DMLEFTTRAY					 L"dev_dm_lefttray"
#define MCF_KEY_DEF_V_AUTOCOLORMODE					L"dev_dm_automaticcolormode"
#define MCF_KEY_DEF_V_PINCOMPATIBILITY				L"dev_dm_pincompatibility"

// +++ _DEVDMODE(members)
#define MCF_KEY_DEF_V_DWFIELDVDM					 L"dev_dwFieldVDM"
#define MCF_KEY_DEF_V_BYPUNCHMODULE					 L"dev_byPunchmodule"
#define MCF_KEY_DEF_V_NINPUTTRAY					 L"dev_nInputTray"
#define MCF_KEY_DEF_V_NOUTPUTTRAY					 L"dev_nOutputTray"
#define MCF_KEY_DEF_V_NINSTALLEDRAM					 L"dev_nInstalledRAM"
#define MCF_KEY_DEF_V_NSTSPSOURCE					 L"dev_nSTSPSource"
#define MCF_KEY_DEF_V_NSTSPSIZE						 L"dev_nSTSPSize"
#define MCF_KEY_DEF_V_NSTSPTYPE						 L"dev_nSTSPType"
#define MCF_KEY_DEF_E2_ADV_EXP_TEXT					L"ext2_adv_exp_text"
#define MCF_KEY_DEF_E2_ADV_EXP_GRAPHICS				L"ext2_adv_exp_graphics"
#define MCF_KEY_DEF_E2_ADV_EXP_PHOTO				L"ext2_adv_exp_photo"

// +++ _DEVDMODE(dwInputTray)
#define MCF_KEY_DEF_V_IN_DMDEVIN1TRAY				 L"dev_dmdev_in_1tray"
#define MCF_KEY_DEF_V_IN_DMDEVIN2TRAY				 L"dev_dmdev_in_2tray"
#define MCF_KEY_DEF_V_IN_DMDEVIN3TRAY				 L"dev_dmdev_in_3tray"
#define MCF_KEY_DEF_V_IN_DMDEVIN4TRAY				 L"dev_dmdev_in_4tray"
#define MCF_KEY_DEF_V_IN_DMDEVIN2TRAYDESK			 L"dev_dmdev_in_2traydesk"
#define MCF_KEY_DEF_V_IN_DMDEVIN3TRAYDESK			 L"dev_dmdev_in_3traydesk"
#define MCF_KEY_DEF_V_IN_DMDEVINLCC1				 L"dev_dmdev_in_lcc1"
#define MCF_KEY_DEF_V_IN_DMDEVINLCC2				 L"dev_dmdev_in_lcc2"
#define MCF_KEY_DEF_V_IN_DMDEVINLCC3				 L"dev_dmdev_in_lcc3"
#define MCF_KEY_DEF_V_IN_DMDEVINLCC4				 L"dev_dmdev_in_lcc4"
#define MCF_KEY_DEF_V_IN_DMDEVINLCC5				 L"dev_dmdev_in_lcc5"
#define MCF_KEY_DEF_V_IN_DMDEVINLCC6				 L"dev_dmdev_in_lcc6"
#define MCF_KEY_DEF_V_IN_DMDEVINBYPASS				 L"dev_dmdev_in_bypass"
#define MCF_KEY_DEF_V_IN_DMDEVINMULTI				 L"dev_dmdev_in_multi"
#define MCF_KEY_DEF_V_IN_DMDEVININSERTER			 L"dev_dmdev_in_inserter"
#define MCF_KEY_DEF_V_IN_DMDEVIN5TRAY				 L"dev_dmdev_in_5tray"
#define MCF_KEY_DEF_V_IN_DMDEVINBYPASS1				 L"dev_dmdev_in_bypass1"
#define MCF_KEY_DEF_V_IN_DMDEVINBYPASS2				 L"dev_dmdev_in_bypass2"
#define MCF_KEY_DEF_V_IN_DMDEVINBYPASS3				 L"dev_dmdev_in_bypass3"
#define MCF_KEY_DEF_V_IN_DMDEVINBYPASS4				 L"dev_dmdev_in_bypass4"

// +++ _DEVDMODE(dwInputTray)
#define MCF_KEY_DEF_V_OUT_DMDEVOUTUPPER				 L"dev_dmdev_out_upper"
#define MCF_KEY_DEF_V_OUT_DMDEVOUTFIN				 L"dev_dmdev_out_finisher"
#define MCF_KEY_DEF_V_OUT_DMDEVOUTSSF				 L"dev_dmdev_out_ssf"
#define MCF_KEY_DEF_V_OUT_DMDEVOUTMAILBIN			 L"dev_dmdev_out_mailbin"
#define MCF_KEY_DEF_V_OUT_DMDEVOUT20BIN				 L"dev_dmdev_out_20bin"
#define MCF_KEY_DEF_V_OUT_DMDEVOUTLEFT				 L"dev_dmdev_out_left"
#define MCF_KEY_DEF_V_OUT_DMDEVOUTSADDLE			 L"dev_dmdev_out_saddle"
#define MCF_KEY_DEF_V_OUT_DMDEVOUTFIN2				 L"dev_dmdev_out_finisher85"
// >> 2008.05.09 SSL:K.Hamaguchi
// for c-jupiter2
#define MCF_KEY_DEF_V_OUT_DMDEVOUTFIN_LS			 L"dev_dmdev_out_fin_ls"
// << 2008.05.09 SSL:K.Hamaguchi
// >> 2009.11.24 SSL:K.Hamaguchi
// for dragon3
#define MCF_KEY_DEF_V_OUT_DMDEVOUTFIN3				 L"dev_dmdev_finisher3trays"
// <<> 2009.11.24 SSL:K.Hamaguchi

// >> 2010.12.08 SSL:K.Hamaguchi
// for virgo
#define MCF_KEY_DEF_V_OUT_DMDEVOUTSSF2				 L"dev_dmdev_out_ssf2"
// << 2010.12.08 SSL:K.Hamaguchi

// +++ JOBCONTROL(wFieldJC)
#define MCF_KEY_DEF_J_DMAUTOJOB						 L"jc_dm_autojob"
#define MCF_KEY_DEF_J_DMNOTIFYJOBEND				 L"jc_dm_notifyjobend"
#define MCF_KEY_DEF_J_DMALWAYSUSEACNUM				 L"jc_dm_alwaysuseacnum"
#define MCF_KEY_DEF_J_DMALWAYSUSEUSERID				 L"jc_dm_alwaysuseuserid"
#define MCF_KEY_DEF_J_DMALWAYSUSEJOBID				 L"jc_dm_alwaysusejobid"
#define MCF_KEY_DEF_J_DMALWAYSUSEFOLDERPIN			 L"jc_dm_alwaysusefolderpin"
#define MCF_KEY_DEF_J_DMDOCFILING					 L"jc_dm_docfiling"
#define MCF_KEY_DEF_J_DMUSEPIN						 L"jc_dm_use_pin"
// <S><A> 2011.04.20 SSL:K.Hamaguchi
#define MCF_KEY_DEF_J_DMALWAYSUSELOGINNAME			 L"jc_dm_alwaysuseloginname"
// <E> 2011.04.20 SSL:K.Hamaguchi

// +++ JOBCONTROL(members)
#define MCF_KEY_DEF_J_BYRETENTION					 L"jc_byRetention"
#define MCF_KEY_DEF_J_BYPINENCRYPTION				 L"jc_byPinEncryption"
#define MCF_KEY_DEF_J_BYDOCUMENTFILING				 L"jc_byDocumentFiling"
#define MCF_KEY_DEF_J_NDOCFILINGFOLDER				 L"jc_nDocFilingFolder"

// +++ COLORDATA(wFieldCD)
#define MCF_KEY_DEF_C_DMBLACKOVERPRN				 L"cd_dm_blackoverprn"
#define MCF_KEY_DEF_C_DMPUREBLACK					 L"cd_dm_pureblack"
#define MCF_KEY_DEF_C_DMGRAYCOMPENSATION			 L"cd_dm_graycompensation"
#define MCF_KEY_DEF_C_DMNEUTRALGRAYBMP				 L"cd_dm_neutralgraybmp"
#define MCF_KEY_DEF_C_DMNEUTRALGRAYGRPH				 L"cd_dm_neutralgraygrph"
#define MCF_KEY_DEF_C_DMNEUTRALGRAYTEXT				 L"cd_dm_neutralgraytext"

// >> 2007.08.02 SBC:Kawabata
#define MCF_KEY_DEF_C_DMCOLSELPRN_K					 L"cd_dm_colselprn_k"
#define MCF_KEY_DEF_C_DMCOLSELPRN_C					 L"cd_dm_colselprn_c"
#define MCF_KEY_DEF_C_DMCOLSELPRN_M					 L"cd_dm_colselprn_m"
#define MCF_KEY_DEF_C_DMCOLSELPRN_Y					 L"cd_dm_colselprn_y"
// << 2007.08.02 SBC:Kawabata

// >> 2008.03.10 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_DEF_C_DMCMYLCORRECTION				 L"cd_dm_cmykcorrection"
// << 2008.03.10 SSL:K.Hamaguchi

// >> 2010.04.12 SSL:K.Hamaguchi
#define MCF_KEY_DEF_C_DMPUREBKPRNBMP				 L"cd_dm_neutralgraybmp"
#define MCF_KEY_DEF_C_DMPUREBKPRNGRPH				 L"cd_dm_neutralgraygrph"
#define MCF_KEY_DEF_C_DMPUREBKPRNTEXT				 L"cd_dm_neutralgraytext"
#define MCF_KEY_DEF_C_DMTRAPPING					 L"cd_dm_trapping"
#define MCF_KEY_DEF_C_DMSHARPNESS					 L"cd_dm_sharpness"
// << 2010.04.12 SSL:K.Hamaguchi

// +++ COLORDATA(members)
#define MCF_KEY_DEF_C_BYDOCTYPE						 L"cd_byDocType"
#define MCF_KEY_DEF_C_BYCOLRENDER					 L"cd_byColRender"
#define MCF_KEY_DEF_C_BYUCRSELECT					 L"cd_byUCRSelect"
#define MCF_KEY_DEF_C_BYSCREENING					 L"cd_byScreening"
#define MCF_KEY_DEF_C_BYSCREENINGBMP				 L"cd_byScreeningBmp"
#define MCF_KEY_DEF_C_BYSCREENINGGRAPH				 L"cd_byScreeningGraph"
#define MCF_KEY_DEF_C_BYSCREENINGTEXT				 L"cd_byScreeningText"
#define MCF_KEY_DEF_C_BYCOLMODE						 L"cd_byColMode"
#define MCF_KEY_DEF_C_BYPRNMODE						 L"cd_byPrnMode"
#define MCF_KEY_DEF_C_BYSPOTCLRMATCH				 L"cd_bySpotColorMathing"
#define MCF_KEY_DEF_C_BYINPUTPROFILE				 L"cd_byInputProfile"
#define MCF_KEY_DEF_C_BYOUTPUTPROFILE				 L"cd_byOutputProfile"
#define MCF_KEY_DEF_C_BYINKSIMULATION				 L"cd_byInkSimulation"
#define MCF_KEY_DEF_C_BYINTENTBMP					 L"cd_byIntentBmp"
#define MCF_KEY_DEF_C_BYINTENTGRAPH					 L"cd_byIntentGraph"
#define MCF_KEY_DEF_C_BYINTENTTEXT					 L"cd_byIntentText"
// >> 2008.01.10 SBC:Kawabata
// for CR
#define MCF_KEY_DEF_C_BYRESOLTYPE					 L"cd_byResolType"
// << 2008.01.10 SBC:Kawabata
#define MCF_KEY_DEF_C_NBRIGHTNESS					 L"cd_nBrightness"
#define MCF_KEY_DEF_C_NCONTRAST						 L"cd_nContrast"
#define MCF_KEY_DEF_C_NSATURATION					 L"cd_nSaturation"
#define MCF_KEY_DEF_C_NREDSTRENGTH					 L"cd_nRedStrength"
#define MCF_KEY_DEF_C_NGREENSTRENGTH				 L"cd_nGreenStrength"
#define MCF_KEY_DEF_C_NBLUESTRENGTH					 L"cd_nBlueStrength"
#define MCF_KEY_DEF_C_NGBRED						 L"cd_nGBRed"
#define MCF_KEY_DEF_C_NGBGREEN						 L"cd_nGBGreen"
#define MCF_KEY_DEF_C_NGBBLUE						 L"cd_nGBBlue"
#define MCF_KEY_DEF_C_NSCREENFREQ					 L"cd_nScreenFreq"
#define MCF_KEY_DEF_C_NSCREENANG					 L"cd_nScreenAng"
#define MCF_KEY_DEF_C_BYINPROFILEDEV				 L"cd_byInputProfileDevice"
#define MCF_KEY_DEF_C_BYOUTPROFILEDEV				 L"cd_byOutputProfileDevice"
// >> 2008.05.26 SSL:K.Hamaguchi
// for Pegasus3 Light
#define MCF_KEY_DEF_C_BYDENSITY						 L"cd_byDensity"

// +++ PCFAXINFO
#define MCF_KEY_DEF_F_WCOLORMODE					 L"pf_wColorMode"
#define MCF_KEY_DEF_F_WCOMPTYPE						 L"pf_wCompType"
#define MCF_KEY_DEF_F_WROTATE						 L"pf_wRotate"
// >> 2010.07.28 SSL:K.Hamaguchi
#define MCF_KEY_DEF_F_BYWOVENINDEX					 L"pf_byWovenIndex"
#define MCF_KEY_DEF_F_BYLAYOUTPRINT					 L"pf_byLayOutPrint"
#define MCF_KEY_DEF_F_BYDUPLEXSTYLE					 L"pf_byDuplexStyle"
// << 2010.07.28 SSL:K.Hamaguchi

// >> 2010.08.06 SSL:K.Hamaguchi
// --- CusImgRegDef
#define MCF_SEC_CIR_DEF								 L"CusImgRegDef"
#define MCF_KEY_CIRDEF_TYPE							 L"cir_type"
#define MCF_KEY_CIRDEF_NAME							 L"cir_name"
// << 2010.08.06 SSL:K.Hamaguchi

// --- PropertySettings
#define MCF_SEC_PROPSETTINGS						 L"PropertySettings"

#ifdef _WIN32

// +++ tab support
#define MCF_KEY_PROP_TABAPP							 L"tab_app"
#define MCF_KEY_PROP_TABAPP_MONO					 L"tab_app_mono"
#define MCF_KEY_PROP_TABAPP_DUMMY					 L"tab_app_dummy"
#define MCF_KEY_PROP_TABPRN							 L"tab_prn"
#define MCF_KEY_PROP_TABAPP_9X						 L"tab_app9x"
#define MCF_KEY_PROP_TABPRN_9X						 L"tab_prn9x"
#define MCF_KEY_PROP_TABAPP_WEB						 L"tab_app_web"
#define MCF_KEY_PROP_TABPRN_WEB						 L"tab_prn_web"
#define MCF_KEY_PROP_TABAPP_FAX						 L"tab_app_fax"
#define MCF_KEY_PROP_TABPRN_FAX						 L"tab_prn_fax"

#define MCF_KEY_PROP_TABHELP						 L"tab_help"


// +++ control support

#define MCF_KEY_PROP_SUP_PAMPH						 L"sup_pamphlet"				// pamphlet support
#define MCF_KEY_PROP_SUP_OUTTRAY					 L"sup_outputtray"			// output tray support
#define MCF_KEY_PROP_SUP_POSTER						 L"sup_poster"				// poster support
#define MCF_KEY_PROP_SUP_STAPLE						 L"sup_staple"				// staple support
#define MCF_KEY_PROP_SUP_PUNCH						 L"sup_punch"					// punch support
#define MCF_KEY_PROP_SUP_DIFFMETHOD					 L"sup_diff_method"			// different paper printing method
// >> 2009.03.23 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SUP_DIFF_L_PSRC				 L"sup_diff_l_psrc"			// different paper paper source last
// << 2009.03.23 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SUP_TABIALIGN					 L"sup_tabi_textalign"		// tab image text align

#define MCF_KEY_PROP_SUP_MIRROR						 L"sup_mirror_image"			// mirror image support
#define MCF_KEY_PROP_SUP_JOBCOMP					 L"sup_job_compression"		// job compression support
#define MCF_KEY_PROP_SUP_DLFORMAT					 L"sup_download_format"		// download format support
#define MCF_KEY_PROP_SUP_LINEW						 L"sup_line_width"			// line width support
// >> 2007.11.14 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_SUP_LINEW_DOT					 L"sup_line_width_dot"		// line width support
// << 2007.11.14 SSL:K.Hamaguchi

#define MCF_KEY_PROP_SUP_COLORMODE					 L"sup_color_mode"			// color mode support
#define MCF_KEY_PROP_SUP_ICMSRC						 L"sup_icm_src"				// ICM source support
#define MCF_KEY_PROP_SUP_ICMDST						 L"sup_icm_dst"				// ICM output support

#define MCF_KEY_PROP_SUP_INTENT						 L"sup_intent"				// intent support
#define MCF_KEY_PROP_SUP_INKSIM						 L"sup_ink_simulation"		// ink simulation support
#define MCF_KEY_PROP_SUP_NEUTRAL_GRAY				 L"sup_neutral_gray"			// neutral gray support
#define MCF_KEY_PROP_SUP_NEUTRAL_GRAY_NOT_CUS		 L"sup_neutral_gray_not_custom"	// neutral gray support (not custom)
#define MCF_KEY_PROP_SUP_SCR_BMP					 L"sup_screening_bmp"			// screening bmp
#define MCF_KEY_PROP_SUP_SCR_GRP					 L"sup_screening_grp"			// screening graphics
#define MCF_KEY_PROP_SUP_SCR_TXT					 L"sup_screening_txt"			// screening text
#define MCF_KEY_PROP_SUP_PRNMODE					 L"sup_print_mode"			// print mode support
// >> 2010.04.12 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SUP_PUREBK						 L"sup_purebke"				// pure black print
#define MCF_KEY_PROP_SUP_SHARPNESS					 L"sup_sharpness"				// sharpness
// << 2010.04.12 SSL:K.Hamaguchi

#define MCF_KEY_PROP_SUP_WMBORDER					 L"sup_wm_border"				// watermark border support
#define MCF_KEY_PROP_SUP_WMCOLOR					 L"sup_wm_color"				// watermark color support

#define MCF_KEY_PROP_SUP_PUNCH_MOD					 L"sup_punch_module"			// punch module option support
#define MCF_KEY_PROP_SUP_COLOR_MODE					 L"sup_colormode"				// color mode option support settings tab

#define MCF_KEY_PROP_FAX_COMP						 L"sup_fax_compression"		// fax compression support

#define MCF_KEY_PROP_MAX_USERSET					 L"max_userset"				// max count of user settings
#define MCF_KEY_PROP_MAX_US_DIFF					 L"max_userset_diff"			// max count of user settings (different paper)
#define MCF_KEY_PROP_MAX_US_TABPRN					 L"max_userset_tabprn"		// max count of user settings (tab print)
#define MCF_KEY_PROP_MAX_WATERMARK					 L"max_watermark"				// max count of watermarks
#define MCF_KEY_PROP_MAX_IMAGESTAMP					 L"max_imagestamp"			// max count of image stamps
#define MCF_KEY_PROP_MAX_DPPAGECOUNT				 L"max_diffpaper_count"		// max count of pages of different paper
// >> 2010.07.28 SSL:K.Hamaguchi
#define MCF_KEY_PROP_MAX_US_WOVEN					 L"max_userset_woven"			// max count of user settings (woven pattern)
// << 2010.07.28 SSL:K.Hamaguchi

#endif	// #ifdef _WIN32

// >> 2008.04.17 SBC:Kawabata
// for c-frontier
#define MCF_KEY_PROP_BASEID_PAPERSIZE				 L"base_papersize"			// paper size base ID
// << 2008.04.17 SBC:Kawabata
#define MCF_KEY_PROP_BASEID_PAPERTYPE				 L"base_papertype"			// paper type base ID
#define MCF_KEY_PROP_BASEID_OUTTRAY					 L"base_outputtray"			// output tray base ID
// >> 2008.01.16 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_BASEID_ICMSRC                   L"base_icm_src"				// icm src base ID
// << 2008.01.16 SSL:K.Hamaguchi

#define MCF_KEY_PROP_SUP_NUP						 L"sup_nup"					// N-Up support
#define MCF_KEY_PROP_SUP_NUPORDER					 L"sup_nup_order"				// N-Up order support

//For SingleUI added by SAIRAM on 28 Jun 2011
#define MCF_KEY_PROP_SUP_PAPERSIZE_DUMMY			 L"sup_papersize_dummy"
#define MCF_KEY_PROP_SUP_PAPERSIZE					 L"sup_papersize"				// Paper size support
#define MCF_KEY_PROP_SUP_FTPPAPERSIZE				 L"sup_ftp_papersize"			// FTP Paper size support
// >> 2007.03.09 SBC:Kawabata
// HPの11x17インチの用紙サイズに対して、SHARPのドライバでも自動的にLedgerで処理するように対応して欲しいという要望が
// 入ってきています。対応検討をお願いします。Canonは対応が出来ているとのことです。
#define MCF_KEY_PROP_SUP_DC_PAPERS					 L"sup_dc_papers"				// Device Capabilitires DC_PAPERS
// << 2007.03.09 SBC:Kawabata
#define MCF_KEY_PROP_SUP_PAPERSRC					 L"sup_papersource"			// Paper source support
#define MCF_KEY_PROP_SUP_PAPERTYPE					 L"sup_papertype"				// Paper type support
#define MCF_KEY_PROP_SUP_CUSTOM_PAPERTYPE			 L"sup_custom_papertype"		// Custom Paper type support
// >> 2007.11.16 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_SUP_CUSTOM_PAPERSIZE			 L"sup_custom_papersize"		// Custom Paper size support
// << 2007.11.16 SSL:K.Hamaguchi

#define MCF_KEY_PROP_SUP_BINEDGE					 L"sup_binding_edge"			// binding edge support

#define MCF_KEY_PROP_SUP_RESOLUTION					 L"sup_resolution"			// resolution support
#define MCF_KEY_PROP_SUP_MARGIN						 L"sup_margin_shift"			// margin shift support

#define MCF_KEY_PROP_SUP_DOCCTRL_COLOR				 L"sup_docctrl_color"			// print color(document control)

#define MCF_KEY_PROP_SUP_FONTSRC					 L"sup_font_source"			// font source support
#define MCF_KEY_PROP_SUP_GRAPHMODE					 L"sup_graphics_mode"			// graphics mode support

#define MCF_KEY_PROP_SUP_ICMMETHOD					 L"sup_icm_method"			// ICM Method support
#define MCF_KEY_PROP_SUP_ICMINTENT					 L"sup_icm_intent"			// ICM Intent support
//#define MCF_KEY_PROP_SUP_BMPCOLSPC					 L"sup_bmp_colorspace"		// Bitmap Color Space support
//#define MCF_KEY_PROP_SUP_GRPCOLSPC					 L"sup_grp_colorspace"		// Graphic Color Space support
//#define MCF_KEY_PROP_SUP_TEXTCOLSPC					 L"sup_text_colorspace"		// Text Color Space support
#define MCF_KEY_PROP_SUP_BMPCOMP					 L"sup_bmp_compression"		// Bitmap Compression support
//#define MCF_KEY_PROP_SUP_JPGSAMP					 L"sup_jpeg_sampling"			// Jpeg Sampling support

#define MCF_KEY_PROP_SUP_LCT						 L"sup_lct"					// large capacity tray support
#define MCF_KEY_PROP_SUP_BYPASSOPT					 L"sup_bypass_opt"			// byapass tray support
#define MCF_KEY_PROP_SUP_INSTRAM					 L"sup_installed_ram"			// installed ram support
#define MCF_KEY_PROP_SUP_ST_PAPERSRC				 L"sup_st_papersource"		// Paper source support(set tray status)
#define MCF_KEY_PROP_SUP_ST_PAPERSIZE				 L"sup_st_papersize"			// set tray status paper size support
#define MCF_KEY_PROP_SUP_ST_PAPERTYPE				 L"sup_st_papertype"			// Paper type support(set tray status)

// >> 2009.04.23 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SUP_DIFF_PAPERTYPE				 L"sup_diff_papertype"		// Paper type support(different paper)
#define MCF_KEY_PROP_SUP_CBN_PAPERTYPE				 L"sup_cbn_papertype"			// Paper type support(carbon copy)
#define MCF_KEY_PROP_SUP_TRANS_PAPERTYPE			 L"sup_trans_papertype"		// Paper type support(transparency inserts)
// << 2009.04.23 SSL:K.Hamaguchi

#define MCF_KEY_PROP_MAX_FIELDVDM					 L"max_fieldvdm"				// max state of fieldvdm
#define MCF_KEY_PROP_MAX_INOPTION					 L"max_inoption"				// max state of input options
#define MCF_KEY_PROP_MAX_OUTOPTION					 L"max_outoption"				// max state of output options

#define MCF_KEY_PROP_HIDECTRLS						 L"hide_ctrls"
// >> 2008.05.19 SSL:K.Hamaguchi
#define MCF_KEY_PROP_GRAYOUTCTRLS					 L"grayout_ctrls"
// << 2008.05.19 SSL:K.Hamaguchi
#define MCF_KEY_PROP_DISCOVERY						 L"Discovery"
#define MCF_KEY_PROP_CREATEPRINTERICON				 L"CreatePrinterIcon"
#define MCF_KEY_PROP_PRINTERCHANGEPERMISSION		 L"PrinterChangePermission"
// >> 2007.01.31 SBC:Kawabata
// punch select type
#define MCF_KEY_PROP_PUNCHSELTYPE					 L"punch_sel_type"			// 0 : punch option / 1 : punch type
// << 2007.01.31 S.Kawabata

// >> 2006.07.31 S.Kawabata
// for html help
#define MCF_KEY_PROP_FORCEHTMLHELP					 L"force_html_help"			// Html help is used forcibly.
// << 2006.07.31 S.Kawabata

// >> 2006.12.04 S.Kawabata
// for html help
#define MCF_KEY_PROP_DELETWINICMONVISTA				 L"delete_winicm_on_vista"		// Delete controls only on vista.
// << 2006.12.04 S.Kawabata

// >> 2007.02.01 SBC:Kawabata
// ifax 拡張キット対応
#define MCF_KEY_PROP_IFAXEXTSUP						 L"ifax_ext_support"			// 
// << 2007.02.01 S.Kawabata

// >> 2007.03.02 SBC:Kawabata
// 解像度・Poster・Fit To Page制限
#define MCF_KEY_PROP_LIM_RES_POS_FTP				 L"lim_res_pos_ftp_on"		//  limit resolution vs poster vs ftp
// << 2007.03.02 S.Kawabata

// >> 2007.03.13 SBC:Kawabata
// Job情報管理者ユーザー最大値
#define MCF_KEY_PROP_MAX_SUSER						 L"max_suser"					//  max number of super user
// << 2007.03.13 S.Kawabata

// >> 2007.08.08 SSL:K.Hamaguchi
#define MCF_KEY_LANDDUPLEX_ENA						 L"landscape_duplex_ena"		// LandScape Duplex Enable
// << 2007.08.08 SSL:K.Hamaguchi

// >> 2007.10.23 SSL:K.Hamaguchi
// GDIプリントA3→A4フィットページにて所々罫線抜けが出る
#define MCF_KEY_PROP_SCALE_MYSELF_EXCEL				 L"scale_myself_excel"		// scale_myself_excel
// << 2007.10.23 SSL:K.Hamaguchi

// >> 2007.10.29 SSL:K.Hamaguchi
#define MCF_KEY_DUPLEXAPP_ENA						 L"duplex_app_ena"			// 両面印刷アプリ対応 有効/無効
// << 2007.10.29 SSL:K.Hamaguchi

// >> 2007.11.14 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_SUP_FOLD						 L"sup_fold"					// 紙折り
// << 2007.11.14 SSL:K.Hamaguchi

// >> 2007.11.22 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_SUP_CMPT_SHADE					 L"sup_cmpt_shade"			// 網掛けパターン
#define MCF_KEY_PROP_SUP_CMPT_SPOOL					 L"sup_cmpt_spool"			// スプール形式
#define MCF_KEY_PROP_SUP_CMPT_REDUCED				 L"sup_cmpt_reduced"			// 縮小方式
#define MCF_KEY_PROP_SUP_CMPT_PRNAREA				 L"sup_cmpt_prnarea"			// 印刷領域
// >> 2007.12.06 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SUP_CMPT_LINEWU				 L"sup_cmpt_linewu"			// 線幅調整単位
// << 2007.12.06 SSL:K.Hamaguchi
// << 2007.11.22 SSL:K.Hamaguchi

// >> 2007.11.29 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_PSIZE_SIZE_DISP				 L"psize_size_disp"			// Custom User Paper Size Width Length Disp
#define MCF_KEY_PROP_PSRC_SIZE_DISP					 L"psrc_size_disp"			// Custom User Paper Src  Width Length Disp
// << 2007.11.29 SSL:K.Hamaguchi

// >> 2007.12.04 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_SUP_PRNPATTERN					 L"sup_prnpattern"			// 印刷パターン
// << 2007.12.04 SSL:K.Hamaguchi

// >> 2008.01.09 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_SUP_PRNTMODE					 L"sup_prntmode"			// print mode support
// << 2008.01.09 SSL:K.Hamaguchi

// >> 2008.03.05 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PROP_COLORPRINT_ALERT				 L"colorprint_alert"		// Color Print Alert Displayed
// << 2008.03.05 SSL:K.Hamaguchi

// >> 2008.06.04 SSL:K.Hamaguchi
// for Pegasus3 Light
#define MCF_KEY_PROP_SUP_CMPT_DENSITY				 L"sup_cmpt_density"		// 
// << 2008.06.04 SSL:K.Hamaguchi

// <S><A> 2011.06.13 sbc hashida
#define MCF_KEY_PROP_SUP_CUSTOMUSERPAPER			 L"sup_custom_ups%d"				// custom user paper size用
// <E> 2011.06.13 sbc hashida

// >> 2008.06.04 SSL:K.Hamaguchi
// for Pegasus3 Light
#define MCF_KEY_PROP_STATUSWIN_LIMUSB				 L"StatusWinLimUSB"		// 
// << 2008.06.04 SSL:K.Hamaguchi

// >> 2008.08.19 SSL:K.Hamaguchi
// for C-Jupiter2 長尺印刷
#define MCF_KEY_PROP_LONG_PAPER_PRINT				 L"long_paper_print"		// 長尺印刷
// << 2008.08.19 SSL:K.Hamaguchi

// >> 2009.08.26 SSL:K.Hamaguchi
// Excel 2007 SP2からの拡大縮小印刷でオートシェープ画像位置がずれる対応　有効／無効
#define MCF_KEY_PROP_EXCEL2007SP2_DMSCALE			 L"scale_ena"				// 有効(1)／無効(0)
// << 2008.08.19 SSL:K.Hamaguchi

// >> 2009.11.18 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SUP_PPOS						 L"sup_printposition"
// << 2009.11.18 SSL:K.Hamaguchi

// >> 2010.01.21 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SCDM_VER_CHK					 L"scdm_ver_chk"			// 0:エラーとしない、1:エラーとする
// << 2010.01.21 SSL:K.Hamaguchi

// >> 2010.02.05 SSL:K.Hamaguchi
#define MCF_KEY_PROP_MFPFSEND_EXE_KICK				 L"mfpfsend_exe_kick"		// 0:Kickしない、1:Kickとする
// << 2010.02.05 SSL:K.Hamaguchi

// >> 2010.07.20 SSL:K.Hamaguchi
#define MCF_KEY_PROP_WVN_TEXT_LIST					 L"sup_wvn_textlist"		// 
#define MCF_KEY_PROP_WVN_SIZE						 L"sup_wvn_size"			// Size
#define MCF_KEY_PROP_WVN_ANGLE						 L"sup_wvn_angle"			// Angle
#define MCF_KEY_PROP_WVN_COVER						 L"sup_wvn_cover"			// Cover Pattern
#define MCF_KEY_PROP_WVN_COLOR						 L"sup_wvn_color"			// Print Color
// << 2010.07.20 SSL:K.Hamaguchi

// >> 2010.07.28 sbc:hashida
#define MCF_KEY_PROP_REG_TYPE						 L"sup_reg_type"			// Custom Image Registration Type
// << 2010.07.28 sbc:hashida
// >> 2010.07.28 sbc:hashida
#define MCF_KEY_PROP_LP_TYPE						 L"sup_lp_type"			// LayoutPrint Type
// << 2010.07.28 sbc:hashida

// >> 2010.07.30 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SUP_CMPT_RIP_STYLE				 L"sup_cmpt_rip_style"	// Compatibility - Rip Style
#define MCF_KEY_PROP_SUP_CMPT_DUP_STYLE				 L"sup_cmpt_dup_style"	// Compatibility - Duplex Style
// << 2010.07.30 SSL:K.Hamaguchi

// >> 2010.08.06 SSL:K.Hamaguchi
#define MCF_KEY_PROP_SUP_TONERSAVE					 L"sup_tonersave"			// Color - toner save
// << 2010.08.06 SSL:K.Hamaguchi

// <S><A> 2011.04.20 SSL:K.Hamaguchi
// ログイン名をユーザー名にコピーする対応
#define MCF_KEY_PROP_LOGIN_USER_COPY_ENA			 L"login_user_copy_ena"	// 有効(1)／無効(0)
// <E> 2011.04.20 SSL:K.Hamaguchi

// <S><A> 2011.05.27 SSL:K.Hamaguchi
// A3原稿を縁なしで作成可能とする設定
#define MCF_KEY_PROP_PRINT_A3_EDGEMAX				 L"print_a3_edgemax"		// 有効(1)／無効(0)
// <E> 2011.05.27 SSL:K.Hamaguchi

// ---> 2005.08.24
// --- ここまで整理（Adobe Reader)
// --- value range
#define MCF_SEC_VALUERANGE							 L"ValueRange"

// +++ PUBDMODE
#define MCF_KEY_VAL_DMORIENT						 L"range_p_dmOrientation"		// orientation
#define MCF_KEY_VAL_DMCOPIES						 L"range_p_dmCopies"			// copies
#define MCF_KEY_VAL_DMCOLOR							 L"range_p_dmColor"			// color
#define MCF_KEY_VAL_DMDUPLEX						 L"range_p_dmDuplex"			// duplex
#define MCF_KEY_VAL_DMCOLLATE						 L"range_p_dmCollate"			// collate
#define MCF_KEY_VAL_DMBPP							 L"range_p_dmBitsPerPel"		// bits per pel

// +++ EXTDMODE
#define MCF_KEY_VAL_E_NOVERLAP						 L"range_e_nOverlap"			// overlap
// >> 2007.11.12 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_VAL_E_WMARGINSHIFT					 L"range_e_wMarginShift"		// margin shift range
// << 2007.11.12 SSL:K.Hamaguchi

// +++ COLORDATA
#define MCF_KEY_VAL_C_NBRIGHTNESS					 L"range_c_nBrightness"		// brightness
#define MCF_KEY_VAL_C_NCONTRAST						 L"range_c_nContrast"			// contrast
#define MCF_KEY_VAL_C_NSATURATION					 L"range_c_nSaturation"		// saturation
#define MCF_KEY_VAL_C_NREDSTR						 L"range_c_nRedStrength"		// red strength
#define MCF_KEY_VAL_C_NGREENSTR						 L"range_c_nGreenStrength"	// green strength
#define MCF_KEY_VAL_C_NBLUESTR						 L"range_c_nBlueStrength"		// blue strength
// >> 2008.01.15 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_VAL_C_NGRAY							 L"range_c_nNGray"		// N Gray
// << 2008.01.15 SSL:K.Hamaguchi
// >> 2010.04.12 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_VAL_C_PUREBK						 L"range_c_nPureBk"			// Pure Black Print
// << 2010.04.12 SSL:K.Hamaguchi

// +++ IMAGE ADJUSTMENT
#define MCF_KEY_VAL_I_NBRIGHTNESS					 L"range_i_nBrightness"		// brightness
#define MCF_KEY_VAL_I_NCONTRAST						 L"range_i_nContrast"			// contrast

// +++ Watermark
#define MCF_KEY_VAL_WM_DWPOSX						 L"range_wm_dwPosX"		// watermark position x
#define MCF_KEY_VAL_WM_DWPOSY						 L"range_wm_dwPosY"		// watermark position y
#define MCF_KEY_VAL_WM_NSIZE						 L"range_wm_nSize"		// watermark size
#define MCF_KEY_VAL_WM_NSIZE_IS						 L"range_wm_nSize_is"		// watermark size
#define MCF_KEY_VAL_WM_NANGLE						 L"range_wm_nAngle"		// watermark angle

#define MCF_KEY_VAL_WM_SHADING						 L"range_wm_nShading"		// watermark shading

// >> 2008.05.26 SSL:K.Hamaguchi
// for Pegasus3 Light
#define MCF_KEY_VAL_CMPT_NDENSITY					 L"range_cmpt_nDensity"	// density
// << 2008.05.26 SSL:K.Hamaguchi

// >> 2008.07.23 SSL:K.Hamaguchi
// 長尺印刷機能対応
#define MCF_KEY_VAL_LP_WIDTH						 L"range_lngpaper_width"
#define MCF_KEY_VAL_LP_LENGTH						 L"range_lngpaper_length"
// << 2008.07.23 SSL:K.Hamaguchi

// >> 2009.11.18 SSL:K.Hamaguchi
#define MCF_KEY_VAL_PPOS_ODDX						 L"range_ppos_oddX"
#define MCF_KEY_VAL_PPOS_ODDY						 L"range_ppos_oddY"
#define MCF_KEY_VAL_PPOS_EVNX						 L"range_ppos_evnX"
#define MCF_KEY_VAL_PPOS_EVNY						 L"range_ppos_evnY"
// << 2009.11.18 SSL:K.Hamaguchi

// >> 2010.07.20 SSL:K.Hamaguchi
#define MCF_KEY_VAL_WP_DENSITY						 L"range_wp_nDensity"
// << 2010.07.20 SSL:K.Hamaguchi

// >> 2011.01.26 sbc hashida
#define MCF_KEY_VAL_GPD_RETRY_1 					 L"gpd_retry_1"	// GetPrinterDataRetry level 0
#define MCF_KEY_VAL_GPD_RETRY_2 					 L"gpd_retry_2"	// GetPrinterDataRetry level 1
// << 2011.01.26 sbc hashida

// --- PrinterOptions
#define MCF_SEC_PRNOPTONS							 L"PrinterOptions"
#define MCF_KEY_PRNOPT_WRITE_TO_JSON                 L"opt_write_to_json"

#define MCF_KEY_PRNOPT_DC_COLORDEVICE				 L"opt_dc_colordevice"		// for DC_COLORDEVICE
#define MCF_KEY_PRNOPT_DC_MAXEXTENT					 L"opt_dc_maxextent"			// for DC_MAXEXTENT
#define MCF_KEY_PRNOPT_DC_MINEXTENT					 L"opt_dc_minextent"			// for DC_MINEXTENT
#define MCF_KEY_PRNOPT_DC_TRUETYPE					 L"opt_dc_truetype"			// for DC_TRUETYPE
#define MCF_KEY_PRNOPT_DC_COLLATE					 L"opt_dc_collate"			// for DC_COLLATE
#define MCF_KEY_PRNOPT_DC_ENUMRES					 L"opt_dc_enumresolution"		// for DC_ENUMRESOLUTIONS

#define MCF_KEY_PRNOPT_OPTNUPMGN					 L"opt_nup_margin"

#define MCF_KEY_PRNOPT_CUSTOM_DEFPAPER				 L"opt_customsize_defpaper"	// custom paper size is default paper size

//#define MCF_KEY_PRNOPT_RANGE_PENW_RATE				 L"opt_range_penw_rate"		// range of pen width

#ifdef _WIN32

#define MCF_KEY_PRNOPT_RANGE_XYZOOM_PER				 L"opt_range_xyzoom_percent"	// range of xy-zoom percent

#define MCF_KEY_PRNOPT_SUP_DRVQUERY_SRC				 L"opt_drvquery_src"			// support DrvQueryColorProfile-QCP_SOURCEPROFILE option

#define MCF_KEY_PRNOPT_DEFSCRN_CUSTOM				 L"opt_defscreen_custom"		// user default for screening

#define MCF_KEY_PRNOPT_SUP_US_EACHDATA				 L"opt_userset_eachdata"		// save usersetting by each data

#endif	// #ifdef _WIN32

#define MCF_KEY_PRNOPT_DIFF_SAVETYPE				 L"opt_diff_savetype"			// how to save different paper information
#define MCF_KEY_PRNOPT_TABIMG_TYPE					 L"opt_tabimg_type"			// tab image dialog type

#define MCF_KEY_PRNOPT_SUP_DOCCTRL					 L"opt_documentcontrol"		// support document control function

// >> 2007.11.22 SSL:K.Hamaguchi
// for cr2
#define MCF_KEY_PRNOPT_COLORMODE_DISABLE			 L"opt_color_mode_disable"	// カラー指定禁止
// << 2007.11.22 SSL:K.Hamaguchi

// >> 2008.07.23 SSL:K.Hamaguchi
// 長尺印刷機能対応
#define MCF_KEY_PRNOPT_LNGPAPER_UNIT				 L"opt_lngpaper_unit"			// Long Paper Dialog Unit表示
// << 2008.07.23 SSL:K.Hamaguchi

// >> 2010.06.03 SSL:K.Hamaguchi
// Windows7 + PowerPoint2007の時に、印刷時のダイアログの親ハンドルをNULLにする
#define MCF_KEY_PRNOPT_PARENTHANDLENULL				 L"opt_parent_handle_null"	//
// << 2010.06.03 SSL:K.Hamaguchi

// >> 2010.6.21 SBC:Y.Yoshimura
// for PC FAX
#define MCF_KEY_CREATE_COVER_SHEET_DISP				 L"opt_CreateCoverSheet_disp"	// CreateCoverSheet チェックボックス表示
// << 2010.6.21 SBC:Y.Yoshimura

// >> 2010.09.03 SSL:K.Hamaguchi
// ログイン名のデフォルトにWindowsログイン名を入れる対応
#define MCF_KEY_PRNOPT_LONINNAME_DEF_WIN			 L"opt_loginname_def_win"		//
// << 2010.09.03 SSL:K.Hamaguchi
// <S><C> 2011.01.17 sbc hashida
// PRINT POSITIONを DM_A3W_PAMPHLET ON 時にPrintPositionの値を補正する。
#define MCF_KEY_PAMPHLET_POSITION                    L"opt_pamphlet_position"
// <E>

// --- AutoConfiguration
#define MCF_SEC_AUTOCONFIG							 L"AutoConfig"

#define MCF_KEY_AC_USE_ACM							 L"ac_use_acm"				// use auto config module or not
#define MCF_KEY_AC_BY_USB							 L"ac_by_usb"					// auto config by usb or not
#define MCF_KEY_AC_PRNNAME_MAX						 L"printer_name_max"			// max size of printer name
#define MCF_KEY_AC_TIMER_INTERVAL					 L"ac_timer_interval"			// interval count of watching flag
#define MCF_KEY_AC_TIMER_TIMEOUT					 L"ac_timer_timeout"			// timeout count of waiting update
#define MCF_KEY_SMON_WRITE_CHECK					 L"smon_write_check"			// interval of check status monitor writing
// >> 2009.03.03 SSL:K.Hamaguchi
#define MCF_KEY_AC_DIR								 L"ac_dir"					// auto config reg dir
// << 2009.03.03 SSL:K.Hamaguchi
// >> 2009.03.12 SSL:K.Hamaguchi
#define MCF_KEY_AC_NOTUSECUSTOM						 L"ACMNotUseCustom"
#define MCF_KEY_AC_IGNORETRAYOPEN					 L"ACMIgnoreTrayOpen"
// << 2009.03.12 SSL:K.Hamaguchi

// >> 2010.07.05 Y.Hashida AutoConfig Retry
#define MCF_KEY_AC_RETRY							 L"ac_retry"
// << 2010.07.05 Y.Hashida AutoConfig Retry

#define MCF_SEC_AUTOCONFIG_MIBOID					 L"MIBOID"
#define MCF_KEY_AC_MIBOID_COUNT						 L"IDCount"					
#define MCF_SEC_AC_SMONPSIZE						 L"SmonPSize"

// >> 2009.03.12 SSL:K.Hamaguchi
#define MCF_SEC_MIB_PS_FEEDCOMB						 L"MIBPSFeedComb"
#define MCF_SEC_MIB_PTYPE_TABLE						 L"MIBPTypeTable"
// << 2009.03.12 SSL:K.Hamaguchi

#define MCF_SEC_AUTOCONFIG_DEVICE					 L"AutoConfigTableDevice"
#define MCF_SEC_AUTOCONFIG_PSIZE					 L"AutoConfigTablePSize"
#define MCF_SEC_AUTOCONFIG_PSRC						 L"AutoConfigTablePSrc"
#define MCF_SEC_AUTOCONFIG_PTYPE					 L"AutoConfigTablePType"

#ifdef _WIN32

// --- ImageInfo
#define MCF_SEC_IMGINFO								 L"ImageInfo"

#define MCF_SEC_IMGINFO_MC_X						 L"machine_x"
#define MCF_SEC_IMGINFO_MC_Y						 L"machine_y"
#define MCF_SEC_IMGINFO_MC_SKIP						 L"img_skip"


#endif	// #ifdef _WIN32

// >> 2008.07.15 SBC:Kawabata
// ユーザー設定プリセット対応
#define MCF_SEC_USPRESET							 L"usersettings"

#define MCF_KEY_DEF_USPRESET_FILE					 L"usersetting_file"
// << 2008.07.15 SBC:Kawabata

// --- webuiinfo
#define MCF_SEC_WUIINFO								 L"webuiinfo"

#define MCF_KEY_WUI_TIMER							 L"webui_timer"
#define MCF_KEY_WUI_MODE							 L"webui_mode"
#define MCF_KEY_WUI_TIMERTIMEOUT					 L"webui_timertimeout"

// >> 2008.01.28 SSL:K.Hamaguchi
// --- Job Name Shortening
#define MCF_SEC_JOBNAMESHORTNING					 L"JobNameShortening"			// ジョブ名短縮

#define MCF_KEY_JNS_COUNT							 L"count"
// << 2008.01.28 SSL:K.Hamaguchi

// >> 2008.01.23 SSL:K.Hamaguchi
// --- ScreeningSetting
#define MCF_SEC_SCREENINGSETTINGS					 L"ScreeningSetting"			// スクリーン設定
// << 2008.01.23 SSL:K.Hamaguchi

// >> 2007.11.29 SSL:K.Hamaguchi
// --- IndividualSetting
#define MCF_SEC_INDIVIDUALSETTINGS					 L"IndividualSetting"			// 個別対応

#define MCF_KEY_IND_OUTPUTTRAY_FIX					 L"output_tray_fix"			// 排出先固定(1:有効、0:無効)

// --- OutputTrayFix
#define MCF_SEC_OUTPUTTRAYFIX						 L"OutputTrayFix"

// << 2007.11.29 SSL:K.Hamaguchi

// >> 2010.05.11 SSL:K.Hamaguchi
// 中間調補正データの取得方法
#define MCF_SEC_GETCALIBRATION						 L"GetCalibration"
#define MCF_KEY_GETCLB_KIND							 L"kind"						// 0:CR2等、1:CR3
#define MCF_KEY_GETCLB_POLLING_MFP_STATE			 L"polling_mfp_state"			// polling
#define MCF_KEY_GETCLB_POLLING_GETCLBDATA			 L"polling_get_clbdata"		// polling
// << 2010.05.11 SSL:K.Hamaguchi


// --- Debug
#define MCF_SEC_DEBUG								 L"Debug"

#define MCF_KEY_DEBUG_AC							 L"dbg_ac_reg"				// レジストリの初期化・実際にACモジュール起動は行わないモード
#define MCF_KEY_DEBUG_SPEC_DLRAM					 L"dbg_specify_download_RAM"	// specify the size of download RAM
#define MCF_KEY_DEBUG_SYNCHRO_BK					 L"dbg_synchronized_black"	// synchronize DM_ALLVECTORBLACK and DM_FILLTOBLACK
#define MCF_KEY_DEBUG_WEBUI_ID						 L"dbg_webui_session_id"		// specify the session id of web ui
#define MCF_KEY_DEBUG_ACMFLAG						 L"dbg_autoconfig_flag"		// 
#define MCF_KEY_DEBUG_AC_ANYSTATE					 L"dbg_autoconfig_anystate"	// ignore state of printer
#define MCF_KEY_DEBUG_AC_ANYDEVICE					 L"dbg_autoconfig_anydevice"	// ignore model name of printer
#define MCF_KEY_DEBUG_AC_ALWAYSDISP					 L"dbg_autoconfig_alwaysdisp"	// always display auto configuration dialog
#define MCF_KEY_DEBUG_US_LOG						 L"dbg_userset_log"			// output log of usersetting
#define MCF_KEY_DEBUG_US_NOT_UPDATE					 L"dbg_userset_notupdate"		// not update user settings even if SCDM is version upped
#define MCF_KEY_DEBUG_AC_PAUSE						 L"dbg_autoconfig_pause"		// pause auto configuration process after ACM succeeded
#define MCF_KEY_DEBUG_SMON_PAUSE					 L"dbg_statusmon_pause"		// pause status monitor process
#define MCF_KEY_DEBUG_EDT_SPINPOS					 L"dbg_editbox_spinpos"		// 
#define MCF_KEY_DEBUG_EDT_PASSWORD					 L"dbg_editbox_xxxxxxxx"		// 
#define MCF_KEY_DEBUG_OUT_DEVMODEBIN				 L"dbg_devmode_bin"			// 
#define MCF_KEY_DEBUG_OUT_HELPINFO					 L"dbg_out_helpinfo"			// Output Help Informations
// >> 2008.12.11 ECS:Shinzato
// mcfファイル作成用デバッグオプション
#define MCF_KEY_DEBUG_MAKE_MCF						 L"dbg_make_mcf"				// Make mcf file
// << 2008.12.11 ECS:Shinzato
// >> 2011.01.14 SSL:K.Hamaguchi
#define MCF_KEY_DEBUG_MGN_SRCDST					 L"dbg_mgn_srcdst"			// 0:規定、1:Src=0、2:Dst=0、3:SrcDst=0
// >> 2011.01.14 SSL:K.Hamaguchi
// <S><C> 2011.01.17 sbc hashida
// PRINT POSITIONを DM_A3W_PAMPHLET ON 時にPrintPositionの値を補正する。
#define MCF_KEY_DEBUG_POSITION                       L"dbg_printpos"
// <E>
#define MCF_KEY_PRNOPT_GPD_RETRY					L"opt_gpd_retry" 
#define MCF_KEY_AC_SINGLE_SIGN_ON					L"ac_Single_Sign_On"
#define MCF_KEY_DEF_V_DMBOOKLETMAKER				L"dev_dm_bookletmaker"

#define MCF_SEC_DEF_CHANGE_LCID                     L"DefaultsChangeLCID"        // 特定LCIDで、各種初期値を変更するセクションを指定する
#define MCF_KEY_SELECT_LCID_TYPE                    L"select_lcid_type"          // 0:System local(default) 1:user local
#define MCF_SEC_DEF_LCID_SECTION                    L"DefaultByNo"               // 
#define RLV_KEY_SUPPORTLANG							L"SupportLang"
#define RLV_KEY_SUPPORTED_LANGIDS					L"SupportedLANGIDs"
#define MCF_KEY_DEF_V_BYCOLORMODULE					L"dev_byColormodule"
#define RLV_KEY_SETUP_TYPE							L"SetupType"
#define MCF_SEC_AUTOCONFIG_FINISHERMODULE			L"AutoConfigTableFinisherModule"
#define MCF_SEC_AUTOCONFIG_PUNCHMODULE				L"AutoConfigTablePunchModule"
#define MCF_KEY_DEF_V_BYSTAPLELESS  				L"dev_byStapleless"
#define MCF_SEC_AUTOCONFIG_DEVDEFALTIGNORE			L"AutoConfigDevDefaltIgnore"
#define MCF_KEY_DEF_V_IN_DMDEVIN4TRAY_L				L"dev_dmdev_in_4tray_l"
#define MCF_KEY_DEF_V_IN_DMDEVINLCC7				L"dev_dmdev_in_lcc7"
#define MCF_KEY_DEF_V_OUT_DMDEVOUTFIN4				L"dev_dmdev_out_finisher4"		// Taurus Inner Finisher
#define MCF_KEY_AC_PRINT_RELEASE					L"ac_print_release"	

// ================================================
// _/_/_/  用紙サイズ情報データファイル
// ================================================
// --- options
#define PID_SEC_PAPEROPTIONS						 L"paper_options"

#define PID_KEY_OPT_FORCEFTP						 L"force_ftp"
#define PID_KEY_OPT_LEF								 L"long_edge_feed_size"

// --- Papers
#define PID_SEC_PAPERS								 L"Papers"

// --- PapersLand
#define PID_SEC_PAPERSLAND							 L"PapersLand"

// --- PaperPixelPort
#define PID_SEC_PAPERPIXEL							 L"PaperPixelPort"

// --- PaperPixelLand
#define PID_SEC_PAPERPIXELLAND						 L"PaperPixelLand"

// --- Margin
#define PID_SEC_MARGIN								 L"Margin"

// --- MarginL
#define PID_SEC_MARGINLAND							 L"MarginL"

// --- PaperPixelPortEdgeToEdge
#define PID_SEC_PAPERPIXEL_ETOE						 L"PaperPixelPortEdgeToEdge"

// --- PaperPixelLandEdgeToEdge
#define PID_SEC_PAPERPIXELLAND_ETOE					 L"PaperPixelLandEdgeToEdge"

// --- Margin
#define PID_SEC_MARGIN_ETOE							 L"MarginE"

// --- MarginL
#define PID_SEC_MARGINLAND_ETOR						 L"MarginLE"

// >> 2011.01.17 SSL:K.Hamaguchi
// --- PaperPixelPortA3WP
#define PID_SEC_PAPERPIXEL_A3WP						 L"PaperPixelPortA3WP"

// --- PaperPixelLandA3WP
#define PID_SEC_PAPERPIXELLAND_A3WP					 L"PaperPixelLandA3WP"

// --- PaperPixelPortA3WP
#define PID_SEC_PAPERPIXEL_A3WP_600					 L"PaperPixelPortA3WP_600"

// --- PaperPixelLandA3WP
#define PID_SEC_PAPERPIXELLAND_A3WP_600				 L"PaperPixelLandA3WP_600"

// --- PaperPixelPortA3WP
#define PID_SEC_PAPERPIXEL_A3WP_1200				 L"PaperPixelPortA3WP_1200"

// --- PaperPixelLandA3WP
#define PID_SEC_PAPERPIXELLAND_A3WP_1200			 L"PaperPixelLandA3WP_1200"

// --- MarginA3WP
#define PID_SEC_MARGIN_A3WP							 L"MarginA3WP"

// --- MarginL
#define PID_SEC_MARGINLAND_A3WP						 L"MarginLA3WP"
// << 2011.01.17 SSL:K.Hamaguchi


// ================================================
// _/_/_/  制限事項テーブルファイル
// ================================================
// --- Settings
#define LIMTBL_SEC_SETTINGS							 L"Settings"

#define LIMTBL_KEY_SET_LOOPMAX						 L"loop_max"

// --- limitations
#define LIMTBL_SEC_PSIZE_PSRC						 L"psize_psrc"
#define LIMTBL_SEC_PSIZE_PTYPE						 L"psize_ptype"
#define LIMTBL_SEC_PSIZE_OUTPUT						 L"psize_output"
#define LIMTBL_SEC_PSIZE_DUPLEX						 L"psize_duplex"
#define LIMTBL_SEC_PSIZE_STAPLE_BINEDGE				 L"psize_staple_binedge"
#define LIMTBL_SEC_PSIZE_PUNCH_BINEDGE				 L"psize_punch_binedge"
#define LIMTBL_SEC_PSIZE_LCT						 L"psize_lct"

#define LIMTBL_SEC_PSIZE_PUNCH_BINEDGE_3HOLES		 L"psize_punch_binedge_3holes"

#define LIMTBL_SEC_PTYPE_DUPLEX						 L"ptype_duplex"
#define LIMTBL_SEC_PTYPE_OUTPUT						 L"ptype_output"
// >> 2008.04.25 SSL:K.Hamaguchi
// for pegasus3 light
#define LIMTBL_SEC_PSRC_DUPLEX						 L"psrc_duplex"
// << 2008.04.25 SSL:K.Hamaguchi

#define LIMTBL_SEC_PSRC_PTYPE						 L"psrc_ptype"
#define LIMTBL_SEC_LCT_PTYPE						 L"lct_ptype"
#define LIMTBL_SEC_DUPLEX_BINEDGE					 L"duplex_binedge"
#define LIMTBL_SEC_STPSRC_PSIZE						 L"stpsrc_psize"
#define LIMTBL_SEC_STLCT_PSIZE						 L"stlct_psize"

#define LIMTBL_SEC_SHORTEDGE_SET					 L"shortedge_settings"
#define LIMTBL_KEY_SES_COUNT						 L"count"

#define LIMTBL_SEC_LONGEDGE_SET						 L"longedge_settings"
#define LIMTBL_KEY_LES_COUNT						 L"count"

// --- staple vs output
#define LIMTBL_SEC_STAPLE_PUNCH_OUTPUT				 L"staple_punch_output"
#define LIMTBL_KEY_SPO_NONE							 L"staple_none"
#define LIMTBL_KEY_SPO_1STAPLE						 L"staple_1"
#define LIMTBL_KEY_SPO_2STAPLE						 L"staple_2"
#define LIMTBL_KEY_SPO_2STAPLE_P					 L"staple_2_pamph"
#define LIMTBL_KEY_SPO_PUNCH						 L"punch"
#define LIMTBL_KEY_SPO_GBCPUNCH						 L"gbc_punch"
#define LIMTBL_KEY_SPO_FOLD							 L"fold"
// >> 2008.02.05 SSL:K.Hamaguchi
#define LIMTBL_KEY_SPO_FLD							 L"fld"		// CR2 Fold
// << 2008.02.05 SSL:K.Hamaguchi
// >> 2008.03.26 SSL:K.Hamaguchi
#define LIMTBL_KEY_SPO_2STAPLE_FLD					 L"staple_2_fold"	// CR2 Fold
// << 2008.03.26 SSL:K.Hamaguchi

// --- restrictions
#define LIMTBL_SEC_RESTRICT							 L"restrictions"

#define LIMTBL_KEY_REST_STAPLE_PTYPE				 L"staple_ptype"
#define LIMTBL_KEY_REST_STAPLE_PAMPH_PTYPE			 L"staple_pamph_ptype"
#define LIMTBL_KEY_REST_PUNCH_PTYPE					 L"punch_ptype"
#define LIMTBL_KEY_REST_FOLDING_PTYPE				 L"folding_ptype"
// >> 2008.02.05 SSL:K.Hamaguchi
#define LIMTBL_KEY_REST_FLD_PTYPE					 L"fld_ptype"	// CR2 Fold
// << 2008.02.05 SSL:K.Hamaguchi

#define LIMTBL_KEY_REST_POSTER_PSIZE				 L"poster_psize"
#define LIMTBL_KEY_REST_POSTER_PTYPE				 L"poster_ptype"

#define LIMTBL_KEY_REST_TPINS_PSIZE					 L"tpins_psize"
#define LIMTBL_KEY_REST_TPINS_PTYPE					 L"tpins_ptype"

// >> 2009.05.28 SSL:K.Hamaguchi
#define LIMTBL_KEY_REST_CCOPY_PSIZE					 L"ccopy_psize"
#define LIMTBL_KEY_REST_CCOPY_PTYPE					 L"ccopy_ptype"
// << 2009.05.28 SSL:K.Hamaguchi

// >> 2009.02.06 SSL:K.Hamaguchi
#define LIMTBL_KEY_REST_FTP_PSIZE					 L"ftp_psize"
#define LIMTBL_KEY_REST_ZOOM_PSIZE					 L"zoom_psize"
// << 2009.02.06 SSL:K.Hamaguchi

#define LIMTBL_KEY_REST_PSRC_WITH_PTYPE				 L"psrc_enable_to_set_with_ptype"

#define LIMTBL_KEY_REST_FAXR_PSIZE					 L"faxr_psize"

// >> 2008.02.07 SSL:K.Hamaguchi
// --- image type vs special option
#define LIMTBL_SEC_IMGTYPE_SPCOPT					 L"imgtype_spcopt"
// << 2008.02.07 SSL:K.Hamaguchi

// >> 2008.03.26 SSL:K.Hamaguchi
// --- staple vs fold
#define LIMTBL_SEC_STAPLE_FLD						 L"staple_fld"
// --- Special Option vs fold
#define LIMTBL_SEC_SPCOPT_FLD						 L"spcopt_fld"
// << 2008.03.26 SSL:K.Hamaguchi

// ================================================
// _/_/_/  言語依存データファイル
// ================================================
// --- WatermarkDefault
#define LANGD_SEC_WMDEFAULT							 L"WatermarkDefault"

#ifdef _WIN32

#define LANGD_KEY_WMDEF_PREBK						 L"preset_black"
#define LANGD_KEY_WMDEF_PRER						 L"preset_red"
#define LANGD_KEY_WMDEF_PREG						 L"preset_green"
#define LANGD_KEY_WMDEF_PREB						 L"preset_blue"
#define LANGD_KEY_WMDEF_PREC						 L"preset_cyan"
#define LANGD_KEY_WMDEF_PREM						 L"preset_magenta"
#define LANGD_KEY_WMDEF_PREY						 L"preset_yellow"
#define LANGD_KEY_WMDEF_PRECUS						 L"preset_custom"

#define LANGD_KEY_WMDEF_OUTLINE_W					 L"outline_w_def"
#define LANGD_KEY_WMDEF_DECORATE_W					 L"decorate_w_def"

#define LANGD_KEY_WMDEF_PREVSIZE					 L"preview_size"

// >> 2007.03.15 SBC:Kawabata
// for Watermark JobInfo
#define LANGD_KEY_WMDEF_JI_UNAME					 L"jobinfo_username"
#define LANGD_KEY_WMDEF_JI_PCNAME					 L"jobinfo_pcname"
#define LANGD_KEY_WMDEF_JI_DATE						 L"jobinfo_date"
#define LANGD_KEY_WMDEF_JI_TIME						 L"jobinfo_time"
#define LANGD_KEY_WMDEF_JI_CUSTOM					 L"jobinfo_custom"
#define LANGD_KEY_WMDEF_JI_STRING					 L"jobinfo_string"
// << 2007.03.15

#endif


#define LANGD_KEY_WMDEF_COUNT						 L"count"


// --- UserSettingDefault
#define LANGD_SEC_USDEFAULT							 L"UserSettingDefault"
#define LANGD_KEY_USDEF_TABSTOP						 L"tab_stop"
#define LANGD_KEY_USDEF_HSCRADJ						 L"h_scroll_adjust"

// --- ImageStampDefault
#define LANGD_SEC_ISDEFAULT							 L"ImageStampDefault"

#define LANGD_KEY_ISDEF_COUNT						 L"count"


// --- OverlayDefault
#define LANGD_SEC_OLDEFAULT							 L"OverlayDefault"

#define LANGD_KEY_OLDEF_COUNT						 L"count"

#ifdef _WIN32

// --- TrayStatusDefault
#define LANGD_SEC_TSDEFAULT							 L"TrayStatusDefault"

#define LANGD_KEY_TSDEF_TABSTOP						 L"tab_stop"
#define LANGD_KEY_TSDEF_PTYPEADJ					 L"papertype_area_adjust"

#endif

// --- DifferentPaperDefault
#define LANGD_SEC_DPDEFAULT							 L"DifferentPaperDefault"
#define LANGD_SEC_DPDEFDATA							 L"DifferentPaper%d"

#define LANGD_KEY_DPDEF_TABSTOP						 L"tab_stop"
#define LANGD_KEY_DPDEF_COUNT						 L"count"

#define LANGD_KEY_DPDEF_TITLE						 L"title"
#define LANGD_KEY_DPDEF_PCOUNT						 L"paper_count"
#define LANGD_KEY_DPDEF_RESID						 L"dpres_id"

// --- InsertChapterDefault
#define LANGD_SEC_CIDEFAULT							 L"InsertChapterDefault"
#define LANGD_SEC_CIDEFDATA							 L"InsertChapter%d"

#define LANGD_KEY_CIDEF_TABSTOP						 L"tab_stop"
#define LANGD_KEY_CIDEF_COUNT						 L"count"

#define LANGD_KEY_CIDEF_TITLE						 L"title"
#define LANGD_KEY_CIDEF_PCOUNT						 L"paper_count"

// --- TabPrintDefault
#define LANGD_SEC_TPDEFAULT							 L"TabPrintDefault"
#define LANGD_SEC_TPDEFDATA							 L"TabPrint%d"

#ifdef _WIN32

#define LANGD_KEY_TPDEF_TABSTOP						 L"tab_stop"
#define LANGD_KEY_TPDEF_FONTMIN						 L"font_min"
#define LANGD_KEY_TPDEF_FONTMAX						 L"font_max"
#define LANGD_KEY_TPDEF_TEXTADJ						 L"text_area_adjust"

#endif

#define LANGD_KEY_TPDEF_FONT						 L"font_def"
#define LANGD_KEY_TPDEF_INDEXMAX					 L"index_count_max"
#define LANGD_KEY_TPDEF_COUNT						 L"count"

#define LANGD_KEY_TPDEF_TITLE						 L"Title"
#define LANGD_KEY_TPDEF_PCOUNT						 L"PaperCount"
#define LANGD_KEY_TPDEF_PSEL						 L"PaperSelect"
#define LANGD_KEY_TPDEF_PSRC						 L"PaperSource"
#define LANGD_KEY_TPDEF_PTYPE						 L"PaperType"
#define LANGD_KEY_TPDEF_PSIZE						 L"PaperSize"
#define LANGD_KEY_TPDEF_ANGLE						 L"TextAngle"
#define LANGD_KEY_TPDEF_OUTUNUSE					 L"OutUnuseTab"
#define LANGD_KEY_TPDEF_ORDER						 L"TabOrder"
#define LANGD_KEY_TPDEF_SHIFT						 L"ImageShift"
#define LANGD_KEY_TPDEF_POS1ST						 L"Pos1stIndex"
#define LANGD_KEY_TPDEF_WIDTH						 L"IndexWidth"
#define LANGD_KEY_TPDEF_LENGTH						 L"IndexLength"
#define LANGD_KEY_TPDEF_INTERVAL					 L"Interval"
#define LANGD_KEY_TPDEF_UNIT						 L"Unit"
#define LANGD_SEC_TPDEF_FONT						 L"Font%d"

// --- ICCProfileDefault
#define LANGD_SEC_ICCDEFAULT						 L"ICCProfileDefault"

#define LANGD_KEY_ICCDEF_AUTOSEL					 L"auto_select"
#define LANGD_KEY_ICCDEF_PROFCOUNT					 L"profile_count"
#define LANGD_KEY_ICCDEF_PROFNAME					 L"profile_name%d"


#define LANGD_SEC_ICCDEFDATA						 L"ICCProf%d"

#define LANGD_KEY_ICCDEF_COUNT						 L"count"

#define LANGD_KEY_ICCDEF_TITLE						 L"title"
#define LANGD_KEY_ICCDEF_SETID						 L"setting_id"
#define LANGD_KEY_ICCDEF_ICMMETHOD					 L"icm_method"
#define LANGD_KEY_ICCDEF_SRCPROF_ID					 L"src_prof_id"
#define LANGD_KEY_ICCDEF_INBMP						 L"input_bmp"
#define LANGD_KEY_ICCDEF_INGRP						 L"input_grp"
#define LANGD_KEY_ICCDEF_INTXT						 L"Input_txt"
#define LANGD_KEY_ICCDEF_OUTPROF_ID					 L"out_prof_id"
#define LANGD_KEY_ICCDEF_OUTBMP						 L"output_bmp"
#define LANGD_KEY_ICCDEF_OUTGRP						 L"output_grp"
#define LANGD_KEY_ICCDEF_OUTTXT						 L"output_txt"
#define LANGD_KEY_ICCDEF_RENDER_ID					 L"render_id"
#define LANGD_KEY_ICCDEF_RENDER_VAL					 L"render_val"
#define LANGD_KEY_ICCDEF_CMYK_SIM					 L"cmyk_sim"
#define LANGD_KEY_ICCDEF_CMYK_SIM_PROF				 L"cmyk_sim_prof"
#define LANGD_KEY_ICCDEF_GRAYBMP					 L"gray_bmp"
#define LANGD_KEY_ICCDEF_GRAYGRP					 L"gray_grp"
#define LANGD_KEY_ICCDEF_GRAYTXT					 L"gray_txt"
#define LANGD_KEY_ICCDEF_PUREBKPRN					 L"pure_bk_prn"
#define LANGD_KEY_ICCDEF_BKOVERPRN					 L"bk_over_prn"
#define LANGD_KEY_ICCDEF_SCRBMP						 L"screen_bmp"
#define LANGD_KEY_ICCDEF_SCRGRP						 L"screen_grp"
#define LANGD_KEY_ICCDEF_SCRTXT						 L"screen_txt"
#define LANGD_KEY_ICCDEF_PRNMODE					 L"prn_mode"
#define LANGD_KEY_ICCDEF_RESID						 L"res_id"

// --- StoredToDefault
#define LANGD_SEC_STRDDEFAULT						 L"StoredToDefault"

#define LANGD_KEY_STRDDEF_COUNT						 L"count"

// >> 2010.07.20 SSL:K.Hamaguchi
// --- Woven Pattern Print
#define LANGD_SEC_WVNDEFAULT						 L"WovenPatternDefault"
#define LANGD_SEC_WVNDEFDATA						 L"WovenPattern%d"

#define LANGD_KEY_WVNDEF_TABSTOP					 L"tab_stop"
#define LANGD_KEY_WVNDEF_COUNT						 L"count"

#define LANGD_KEY_WVNDEF_TITLE						 L"title"
#define LANGD_KEY_WVNDEF_SET_NUM					 L"set_number"
#define LANGD_KEY_WVNDEF_FIXEDTXTLIST				 L"fixed_text_list"
#define LANGD_KEY_WVNDEF_FIXEDTXTLIST_INDEX			 L"fixed_text_list_index"
#define LANGD_KEY_WVNDEF_ARBITRARY_TXT				 L"arbitrary_text"
#define LANGD_KEY_WVNDEF_ARBITRARY_TXT_STR			 L"arbitrary_text_str"
#define LANGD_KEY_WVNDEF_DATA_TIME					 L"date_time"
#define LANGD_KEY_WVNDEF_LNAME_UNUM					 L"lname_unumber"
#define LANGD_KEY_WVNDEF_SERIAL_NUM					 L"serial_number"
#define LANGD_KEY_WVNDEF_AC_JOB_ID					 L"account_job_id"
#define LANGD_KEY_WVNDEF_SIZE						 L"size"
#define LANGD_KEY_WVNDEF_ANGLE						 L"angle"
#define LANGD_KEY_WVNDEF_EFFECT						 L"effect_pattern"
#define LANGD_KEY_WVNDEF_COVER						 L"cover_pattern"
#define LANGD_KEY_WVNDEF_DENSITY					 L"density"
#define LANGD_KEY_WVNDEF_COLOR						 L"print_color"
// << 2010.07.20 SSL:K.Hamaguchi

// --- Debug
#define LANGD_SEC_DEBUG								 L"Debug"

#define LANGD_KEY_DEBUG_SHOWTABSTOPS				 L"dbg_show_tabstops"			// show tabstops of listbox control


// ================================================
// _/_/_/  フォントデータファイル
// ================================================
#define FONTD_SEC_DEF_FONT							 L"DefaultFontSetting"

#define FONTD_KEY_LFHEIGHT							 L"lfHeight"
#define FONTD_KEY_LFWIDTH							 L"lfWidth"
#define FONTD_KEY_LFESCAPEMENT						 L"lfEscapement"
#define FONTD_KEY_LFWEIGHT							 L"lfWeight"
#define FONTD_KEY_LFORIENTATION						 L"lfOrientation"
#define FONTD_KEY_LFITALIC							 L"lfItalic"
#define FONTD_KEY_LFUNDERLINE						 L"lfUnderline"
#define FONTD_KEY_LFSTRIKEOUT						 L"lfStrikeOut"
#define FONTD_KEY_LFCHARSET							 L"lfCharSet"
#define FONTD_KEY_LFOUTPRECISION					 L"lfOutPrecision"
#define FONTD_KEY_LFCLIPPRECISION					 L"lfClipPrecision"
#define FONTD_KEY_LFQUALITY							 L"lfQuality"
#define FONTD_KEY_LFPITCHANDFAMILY					 L"lfPitchAndFamily"
#define FONTD_KEY_LFFACENAME						 L"lfFaceName"


#define FONTD_SEC_FONTLIST_9X						 L"FontList9x"
#define FONTD_SEC_FONTNAME_9X						 L"FontName9X"

#ifdef _WIN32

#define FONTD_SEC_FONTLIST_NT						 L"FontListNT"
#define FONTD_SEC_FONTNAME_NT						 L"FontNameNT"

#endif

#define FONTD_KEY_FONTLIST_COUNT					 L"FontListCount"
#define FONTD_KEY_FONTNAME_COUNT					 L"FontNameCount"
#define FONTD_KEY_FONTDATA_COUNT					 L"FontDataCount"

#define FONTD_KEY_CMDFONT							 L"CmdFont"
#define FONTD_KEY_FONTWEIGHT						 L"FontWeight"
#define FONTD_KEY_FONTITALIC						 L"FontItalic"
#define FONTD_KEY_FONTCHARSET						 L"FontCharset"
#define FONTD_KEY_FONTSYMSETID						 L"FontSymSetID"
#define FONTD_KEY_FONTOPTPITCH						 L"FontOptPitch"
#define FONTD_KEY_FONTOPTDLFONT						 L"FontOptDLFont"


// ================================================
// _/_/_/  TabPrintデータファイル
// ================================================
#define TABPD_SEC									 L"TabPrint"

#define TABPD_KEY_VERSION							 L"Version"
#define TABPD_KEY_TITLE								 L"SettingName"
#define TABPD_KEY_PAPERCOUNT						 L"PaperCount"
#define TABPD_KEY_PAGE								 L"Page"
#define TABPD_KEY_FILEPATH							 L"FilePath"
#define TABPD_KEY_INSTAB							 L"InsertTab"
#define TABPD_KEY_TABORDER							 L"TabOrder"
#define TABPD_KEY_PSIZE								 L"PaperSize"
#define TABPD_KEY_NUMOFINDEX						 L"NumberOfIndex"
#define TABPD_KEY_POS1STINDEX						 L"Pos1stIndex"
#define TABPD_KEY_INDEXWIDTH						 L"IndexWidth"
#define TABPD_KEY_INDEXLENGTH						 L"IndexLength"
#define TABPD_KEY_INTERVAL							 L"Interval"
#define TABPD_KEY_IMAGESHIFT						 L"ImageShift"
#define TABPD_KEY_UNIT								 L"Unit"
#define TABPD_KEY_OUTTAB							 L"OutUnuseTab"
#define TABPD_KEY_PSELECT							 L"PaperSelect"
#define TABPD_KEY_PSRC								 L"PaperSource"
#define TABPD_KEY_PTYPE								 L"PaperType"
#define TABPD_KEY_TEXTALIGN							 L"TextAlignment"
#define TABPD_KEY_TEXTORI							 L"TextOrientation"
#define TABPD_KEY_TEXTANGLE							 L"TextAngle"
#define TABPD_KEY_FT_HEIGHT							 L"FontSizeDef"
#define TABPD_KEY_FT_WEIGHT							 L"FontWeightDef"
#define TABPD_KEY_FT_ITALIC							 L"FontItalicDef"
#define TABPD_KEY_FT_ULINE							 L"FontULineDef"
#define TABPD_KEY_FT_SOUT							 L"FontSOutDef"
#define TABPD_KEY_FT_CHARSET						 L"FontCharSetDefs"
#define TABPD_KEY_FT_OUTPRE							 L"FontOutPreDef"
#define TABPD_KEY_FT_CLIPPRE						 L"FontClipPreDef"
#define TABPD_KEY_FT_QUALITY						 L"FontQualityDef"
#define TABPD_KEY_FT_PANDF							 L"FontPAndFDef"
#define TABPD_KEY_FT_FACENAME						 L"FontFaceName"
#define TABPD_KEY_LIST_TEXT							 L"text"
#define TABPD_KEY_UNICODE							 L"unicode"


// ================================================
// _/_/_/  rlv.dat ファイル
// ================================================
// --- instllation
#define RLV_SEC_INSTLLATION							 L"Installation"

#define RLV_KEY_INST_MODELNAME						 L"ModelName"
#define RLV_KEY_INST_RELEASEVER						 L"ReleaseVersion"
#define RLV_KEY_INST_RELEASEEXT						 L"ReleaseExtention"
#define RLV_KEY_INST_RELEASEBUI						 L"ReleaseBuildDate"
#define RLV_KEY_INST_COPYRIGHT_1					 L"DriverCopyRight1"
#define RLV_KEY_INST_COPYRIGHT_2					 L"DriverCopyRight2"
#define RLV_KEY_INST_COPYRIGHT_3					 L"DriverCopyRight3"
#define RLV_KEY_INST_COPYRIGHT_4					 L"DriverCopyRight4"

// --- each model
#define RLV_KEY_EACH_MODEL							 L"ModelName"
#define RLV_KEY_MODELID								 L"model_ID"
//<S><A> #323in RedMine, 2012.05.28, SSDI:Sujan
//Common Registry Implementation
#define RLV_KEY_COMMONSETTING                        L"CommonSetting"
//<E>#323in RedMine, 2012.05.28, SSDI:Sujan
//<S><A> RedMine Ticket(#604) Individualized Special UD2 Driver, 2013.05.02, SSDI:Padma
#define RLV_KEY_DEFAULTLANG							 L"DefaultLang"
//<E> RedMine Ticket(#604) Individualized Special UD2 Driver, 2013.05.02, SSDI:Padma
#define RLV_KEY_EMUID								 L"emulation_ID"
#define RLV_KEY_INI_MCF								 L"ini_mcf"
#define RLV_KEY_INI_PINF							 L"ini_paperinf"
#define RLV_KEY_INI_LANG							 L"ini_language"
#define RLV_KEY_INI_FONT							 L"ini_font"
#define RLV_KEY_INI_RLV								 L"ini_rlv"
#define RLV_KEY_INI_LIMIT							 L"ini_limit"
#define RLV_KEY_PPM									 L"ppm"
#define RLV_KEY_HIDECTRLS							 L"hide_ctrls"
// >> 2008.05.19 SSL:K.Hamaguchi
#define RLV_KEY_GRAYOUTCTRLS						 L"grayout_ctrls"
// << 2008.05.19 SSL:K.Hamaguchi

// >> 2007.11.26 SSL:K.Hamaguchi
#define RLV_KEY_OUTPUT_UNICODE						 L"output_unicode"
// << 2007.11.26 SSL:K.Hamaguchi

// >> 2008.04.25 SSL:K.Hamaguchi
// for pegasus3 light
#define RLV_KEY_DMDEVINPUT							 L"dev_dmdev_input"
// << 2008.04.25 SSL:K.Hamaguchi

// >> 2008.06.04 SSL:K.Hamaguchi
// for pegasus3 light
#define RLV_KEY_IMC_MODEL							 L"imc_model"
// << 2008.06.04 SSL:K.Hamaguchi

// >> 2008.07.03 SSL:K.Hamaguchi
// for pegasus3 light
#define RLV_KEY_DEF_V_DMDUPLEXMODULE				 L"dev_dm_duplexmodule"
// << 2008.07.03 SSL:K.Hamaguchi

// >> 2009.02.23 SSL:K.Hamaguchi
// for PegasusE Refresh
#define RLV_KEY_INOPTMDL							 L"inopt_model"
// << 2009.02.23 SSL:K.Hamaguchi

// >> 2009.05.28 SSL:K.Hamaguchi
// for Andromeda2 Refresh
#define RLV_KEY_DITHERID							 L"DitherID"
// << 2009.05.28 SSL:K.Hamaguchi

// >> 2010.11.30 SSL:K.Hamaguchi
// for Virgo
#define RLV_KEY_CENTERTRAY_ENA						 L"CenterTrayEna"
// << 2010.11.30 SSL:K.Hamaguchi
#define RLV_KEY_DISCOVERY							 L"Discovery"
#define RLV_KEY_CREATEPRINTERICON					 L"CreatePrinterIcon"
#define RLV_KEY_PRINTERCHANGEPERMISSION				 L"PrinterChangePermission"

#ifdef _WIN32

// ================================================
// _/_/_/  help id
// ================================================
#define HID_SEC_HELP_ID								 L"help ID"
#define HID_SEC_HELP_ID_GDI							 L"Replace_GDI"
#define HID_SEC_HELP_ID_SPLC						 L"Replace_SPLC"
#define HID_SEC_HELP_ID_PCL5						 L"Replace_PCL5"
#define HID_SEC_HELP_ID_PCL6						 L"Replace_PCL6"
#define HID_SEC_HELP_ID_PS							 L"Replace_PS"

#endif	// #ifdef _WIN32

// ================================================
// _/_/_/  Internet Fax 拡張キット
// ================================================
#define IFEKI_SEC_SETTINGS							 L"Settings"

#define IFEKI_KEY_TO								 L"TO"
#define IFEKI_KEY_MODEL								 L"model"

#define IFEKD_SEC_IFAX								 L"IFAX"

#define IFEKD_KEY_NUM								 L"num"
#define IFEKD_KEY_MODEL								 L"model%d"

// ================================================
// _/_/_/  １ブロックデータ数
// ================================================
#define INI_NUM_MCF_SUP_MGN							5
#define INI_NUM_MCF_SUP_BMPCOMP						4


// >> 2008.05.26 SSL:K.Hamaguchi
// for Pegasus3 Light
// ================================================
// _/_/_/  exposure.dat ファイル
// ================================================
#define EPS_SEC_SETTINGS						 L"Settings"

#define EPS_KEY_DENSITY_FOR_FILE				 L"density_for_file"
#define EPS_KEY_COUNT							 L"count"

#define EPS_SEC_EXPOSURE_GRP_C					 L"Exposure%d_Grp_C"
#define EPS_SEC_EXPOSURE_GRP_M					 L"Exposure%d_Grp_M"
#define EPS_SEC_EXPOSURE_GRP_Y					 L"Exposure%d_Grp_Y"
#define EPS_SEC_EXPOSURE_GRP_K					 L"Exposure%d_Grp_K"
#define EPS_SEC_EXPOSURE_PHT_C					 L"Exposure%d_Pht_C"
#define EPS_SEC_EXPOSURE_PHT_M					 L"Exposure%d_Pht_M"
#define EPS_SEC_EXPOSURE_PHT_Y					 L"Exposure%d_Pht_Y"
#define EPS_SEC_EXPOSURE_PHT_K					 L"Exposure%d_Pht_K"
#define EPS_SEC_EXPOSURE_MONO_K					 L"Exposure%d_Mono_K"

#define EPS_SEC_EXPOSURE_GRP_C_1200				 L"Exposure%d_Grp_C_1200"
#define EPS_SEC_EXPOSURE_GRP_M_1200				 L"Exposure%d_Grp_M_1200"
#define EPS_SEC_EXPOSURE_GRP_Y_1200				 L"Exposure%d_Grp_Y_1200"
#define EPS_SEC_EXPOSURE_GRP_K_1200				 L"Exposure%d_Grp_K_1200"
#define EPS_SEC_EXPOSURE_PHT_C_1200				 L"Exposure%d_Pht_C_1200"
#define EPS_SEC_EXPOSURE_PHT_M_1200				 L"Exposure%d_Pht_M_1200"
#define EPS_SEC_EXPOSURE_PHT_Y_1200				 L"Exposure%d_Pht_Y_1200"
#define EPS_SEC_EXPOSURE_PHT_K_1200				 L"Exposure%d_Pht_K_1200"
#define EPS_SEC_EXPOSURE_MONO_K_1200			 L"Exposure%d_Mono_K_1200"
// << 2008.05.26 SSL:K.Hamaguchi

//############## PC DEFINATIONS#############################

#define SHADE_PATTERN_STANDARD					 L"Standard"
#define SHADE_PATTERN_FINE						 L"Fine"
#define SPOOL_FORMAT_EMF						 L"EMF"
#define SHADE_PATTERN_RAW						 L"RAW"
#define PRINTABLEAREA_STANDARD					 L"Standard"
#define PRINTABLEAREA_MAXIMUM					 L"Maximum"
#define LINEWIDTHUNIT_FIXEDWIDTH				 L"FixedWidth"
#define LINEWIDTHUNIT_RATIO						 L"Ratio"

#define MCF_SEC_TABTBNCONTROL						L"TabBtnControl"
#define MCF_KEY_TABTBNCONTROL_TABCOUNT				L"TabCount"					// デフォルト  0
#define MCF_KEY_TABTBNCONTROL_DISPCOUNT				L"DispCount"					// デフォルト  0
#define MCF_KEY_TABTBNCONTROL_STEP					L"Step"						// デフォルト  0
#define MCF_KEY_TABTBNCONTROL_TABBTNINDEX			L"TabBtnIndex"				// デフォルト  0
#define MCF_KEY_TABTBNCONTROL_HIDEBTNFLG			L"HideBtnFlg"				// デフォルト  0(非表示ボタンなし)
#define MCF_KEY_TABTBNCONTROL_DISPLEFT				L"DispLeft"					// デフォルト  1(SCUI_STATE_SHOW)  (非表示:0(SCUI_STATE_OFF)) 
#define MCF_KEY_TABTBNCONTROL_DISPRIGHT				L"DispRight"					// デフォルト 17(SCUI_STATE_ON)    (非表示:0(SCUI_STATE_OFF)) 

#define MCF_SEC_LIMIT_MSG_CONTROL_TBL				L"limit_msg_control_tbl"

#define MCF_KEY_PRNOPT_LMTBYOPTHIDEUI				L"opt_LimitByOption_HideUI" //The flag of configuration limit flow
//<S><A>To Implement Task#3132,15-05-2024,SSDI:Manoj S
//Delete Print Release and Document Filing from Printing Policy
#define MCF_SEC_OVERRIDE_MFPIF_VAL					L"OverrideMfpifVal"				// Override Mfpif Val
#define MFPIF_KEY_FORCE_PRINTRELEASE				L"force-printrelease"				// Printing Policy - Print Release
#define MFPIF_KEY_FORCE_RETENTION					L"force-retention"					// Printing Policy - Print Release
#define MCF_KEY_DEF_V_PP_PRINTRELEASE				L"dev_dm_printrelease"
//<E>To Implement Task #3132,15-05-2024,SSDI:Manoj S
//<S><A>To Fix Redmine #7483,07-06-2024,SSDI:Manoj S
#define WEB_PRINTRELEASE							L"printrelease"	
//<E>To Fix Redmine #7483,07-06-2024,SSDI:Manoj S
#endif	// _INIKEY_H
