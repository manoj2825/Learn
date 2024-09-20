// ============================================================================
// jsonkey.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.02.16
// ============================================================================

#ifndef _JSONKEY_H
#define _JSONKEY_H

#define JSON_SHARPW				L"\\SHARP"
#define JSON_EXTW				L".json"


// ================================================
// _/_/_/  function name for save json file
// ================================================
#define JSON_FUNCTION_NAMEW_NULL			L"null"
#define JSON_FUNCTION_NAMEW_US				L"us"
#define JSON_FUNCTION_NAMEW_CCP				L"ccp"
#define JSON_FUNCTION_NAMEW_DP				L"dp"
#define JSON_FUNCTION_NAMEW_ICC				L"icc"
#define JSON_FUNCTION_NAMEW_IS				L"is"
#define JSON_FUNCTION_NAMEW_JC				L"jc"
#define JSON_FUNCTION_NAMEW_OL				L"ol"
#define JSON_FUNCTION_NAMEW_SPRT			L"sprt"
#define JSON_FUNCTION_NAMEW_US				L"us"
#define JSON_FUNCTION_NAMEW_WM				L"wm"
#define JSON_FUNCTION_NAMEW_WVN				L"wvn"
#define JSON_FUNCTION_NAMEW_TP				L"tp"

// ================================================
// _/_/_/  user setting
// ================================================
#define JSON_KEY_US_ROOT_BASE					L"printer_ui\\user_setting"
#define JSON_KEY_US_INDEX_BASE					L"printer_ui\\user_setting\\%ld"
#define JSON_KEY_US_EACH_ID						L"printer_ui\\user_setting\\each_data"
#define JSON_KEY_US_EACH_ID_BASE				L"printer_ui\\user_setting\\each_data\\%ld"
#define JSON_KEY_US_EACH_ID_UPDATE				L"printer_ui\\user_setting\\each_data\\%ld\\update"

#define JSON_ENT_US_COUNT						"count"
#define JSON_ENT_US_SCDM_VERSION				L"scdm_version"
#define JSON_ENT_US_SCDM_SIZE					L"scdm_size"
#define JSON_ENT_US_LOCALE_PSIZE				L"locale_papersize"
#define JSON_ENT_US_LOCALE_UNIT					L"locale_unit"

#define JSON_ENT_US_MCF_TIMESTAMP				L"mcf_timestamp"

#define JSON_ENT_US_SELECT_LCID					L"dfp_select_lcid" // Default Printer Value Select LCID 

#define JSON_ENT_US_TITLE		L"title"
#define JSON_ENT_US_DATA		L"data"
#define JSON_ENT_US_ID			L"id"
#define JSON_ENT_US_TIME		L"time"
#define JSON_ENT_US_SELF		L"self"

#define JSON_ENT_US_CHAPTERINSPG					L"chapterins_page"

// ================================================
// _/_/_/  margin shift
// ================================================
#define JSON_KEY_MS_ROOT_BASE					L"printer_ui\\margin_shift"
#define JSON_KEY_MS_ROOT_BASE_US				L"printer_ui\\user_setting\\%ld\\margin_shift"
#define JSON_KEY_MS_ROOT_BASE_US_EACH			L"printer_ui\\user_setting\\each_data\\%ld\\margin_shift"
#define JSON_KEY_MS_INDEX_BASE					L"printer_ui\\margin_shift\\%ld"
#define JSON_KEY_MS_INDEX_BASE_US				L"printer_ui\\user_setting\\%ld\\margin_shift\\%ld"
#define JSON_KEY_MS_INDEX_BASE_US_EACH			L"printer_ui\\user_setting\\each_data\\%ld\\margin_shift\\%ld"

#define JSON_ENT_MS_COUNT						L"count"

#define JSON_ENT_MS_DWVAL						L"dwVal"
#define JSON_ENT_MS_DWMILLI						L"dwMilli"
#define JSON_ENT_MS_DWINCH						L"dwInch"
#define JSON_ENT_MS_DWDPI						L"dwDPI"
#define JSON_ENT_MS_DWPIXEL						L"dwPixel"
#define JSON_ENT_MS_DWUNIT						L"dwUnit"

// ================================================
// _/_/_/  user paper size
// ================================================
#define JSON_KEY_UPS_ROOT_BASE					L"printer_ui\\user_paper_size"
#define JSON_KEY_UPS_ROOT_BASE_US				L"printer_ui\\user_setting\\%ld\\user_paper_size"
#define JSON_KEY_UPS_ROOT_BASE_US_EACH			L"printer_ui\\user_setting\\each_data\\%ld\\user_paper_size"
#define JSON_KEY_UPS_INDEX_BASE					L"printer_ui\\user_paper_size\\%ld"
#define JSON_KEY_UPS_INDEX_BASE_US				L"printer_ui\\user_setting\\%ld\\user_paper_size\\%ld"
#define JSON_KEY_UPS_INDEX_BASE_US_EACH			L"printer_ui\\user_setting\\each_data\\%ld\\user_paper_size\\%ld"

#define JSON_ENT_UPS_COUNT						L"count"

#define JSON_ENT_UPS_DWWIDTH					L"dwWidth"
#define JSON_ENT_UPS_DWLENGTH					L"dwLength"
#define JSON_ENT_UPS_WUNIT						L"wUnit"

// ================================================
// _/_/_/  print position
// ================================================
#define JSON_KEY_PP_ROOT_BASE					L"printer_ui\\print_position"
#define JSON_KEY_PP_ROOT_BASE_US				L"printer_ui\\user_setting\\%ld\\print_position"
#define JSON_KEY_PP_ROOT_BASE_US_EACH			L"printer_ui\\user_setting\\each_data\\%ld\\print_position"

#define JSON_ENT_PP_COUNT						L"count"

#define JSON_ENT_PP_WODDX						L"wOddX"
#define JSON_ENT_PP_WODDY						L"wOddY"
#define JSON_ENT_PP_WEVENX						L"wEvenX"
#define JSON_ENT_PP_WEVENY						L"wEvenY"
#define JSON_ENT_PP_DWUNIT						L"dwUnit"

// ================================================
// _/_/_/  trimming
// ================================================
#define JSON_KEY_TR_ROOT_BASE					L"printer_ui\\trimming"
#define JSON_KEY_TR_ROOT_BASE_US				L"printer_ui\\user_setting\\%ld\\trimming"
#define JSON_KEY_TR_ROOT_BASE_US_EACH			L"printer_ui\\user_setting\\each_data\\%ld\\trimming"

#define JSON_ENT_TR_COUNT						L"count"
#define JSON_ENT_TR_DWWIDTHMILLI				L"dwWidthMilli"
#define JSON_ENT_TR_DWWIDTHINCH					L"dwWidthInch"
#define JSON_ENT_TR_DWWIDTHDPI					L"dwWidthDPI"
#define JSON_ENT_TR_DWWIDTHPIXEL				L"dwWidthPixel"
#define JSON_ENT_TR_DWUNIT						L"dwUnit"

// ================================================
// _/_/_/  Custom Pamphlet
// ================================================
#define JSON_KEY_CPM_ROOT_BASE					L"printer_ui\\custom_pamphlet"
#define JSON_KEY_CPM_ROOT_BASE_US				L"printer_ui\\user_setting\\%ld\\custom_pamphlet"
#define JSON_KEY_CPM_ROOT_BASE_US_EACH			L"printer_ui\\user_setting\\each_data\\%ld\\custom_pamphlet"

#define JSON_ENT_CPM_COUNT						L"count"
#define JSON_ENT_CPM_NPAPERSIZEFIT				L"nPaperSizeFit"
#define JSON_ENT_CPM_NPAPERSIZE					L"nPaperSize"
#define JSON_ENT_CPM_DWOUTWIDTHPIXEL			L"dwOutWidthPixel"
#define JSON_ENT_CPM_DWOUTHEIGHTPIXEL			L"dwOutHeightPixel"
#define JSON_ENT_CPM_NOUTLEFTMARGINPIXEL		L"nOutLeftMarginPixel"
#define JSON_ENT_CPM_NOUTTOPMARGINPIXEL			L"nOutTopMarginPixel"

// ================================================
// _/_/_/  Different Paper setting
// ================================================
#define JSON_KEY_DP_ROOT_BASE					L"printer_ui\\different_paper"
#define JSON_KEY_DP_INDEX_BASE					L"printer_ui\\different_paper\\%ld"
#define JSON_KEY_DP_INDEX_INDEX_BASE			L"printer_ui\\different_paper\\%ld\\%ld"

#define JSON_ENT_DP_COUNT						L"count"

#define JSON_ENT_DP_BITFIELD					L"field"
#define JSON_ENT_DP_PAGE						L"page"
#define JSON_ENT_DP_PAPERSEL					L"paper_selection"
#define JSON_ENT_DP_PAPERSRC					L"paper_source"
#define JSON_ENT_DP_PAPERTYPE					L"paper_type"
#define JSON_ENT_DP_PAPERSIZE					L"paper_size"
#define JSON_ENT_DP_METHOD						L"printing_method"

#define JSON_ENT_DP_PAPERCOUNT					L"paper_count"
#define JSON_ENT_DP_TITLE						L"title"

// ================================================
// _/_/_/  tab print setting
// ================================================
#define JSON_KEY_TP_ROOT_BASE					L"printer_ui\\tab_print"
#define JSON_KEY_TP_INDEX_BASE					L"printer_ui\\tab_print\\%ld"
#define JSON_KEY_TP_INDEX_INDEX_BASE			L"printer_ui\\tab_print\\%ld\\%ld"

#define JSON_ENT_TP_LAST_PATH					L"LastPath"

#define JSON_ENT_TP_COUNT						L"count"

#define JSON_ENT_TP_PAGE						L"page"
#define JSON_ENT_TP_METHOD						L"printing_method"
#define JSON_ENT_TP_VERTICAL					L"vertical"
#define JSON_ENT_TP_TEXTPOS						L"text_position"
#define JSON_ENT_TP_TEXT						L"text"
#define JSON_ENT_TP_FNT_LFHEIGHT				L"fnt_lfHeight"
#define JSON_ENT_TP_FNT_LFWIDTH					L"fnt_lfWidth"
#define JSON_ENT_TP_FNT_LFESC					L"fnt_lfEscapement"
#define JSON_ENT_TP_FNT_LFORI					L"fnt_lfOrientation"
#define JSON_ENT_TP_FNT_LFWEIGHT				L"fnt_lfWeight"
#define JSON_ENT_TP_FNT_LFITALIC				L"fnt_lfItalic"
#define JSON_ENT_TP_FNT_LFUNDERLINE				L"fnt_lfUnderline"
#define JSON_ENT_TP_FNT_LFSTRIKEOUT				L"fnt_lfStrikeOut"
#define JSON_ENT_TP_FNT_LFCHARSET				L"fnt_lfCharSet"
#define JSON_ENT_TP_FNT_LFOUTPRE				L"fnt_lfOutPrecision"
#define JSON_ENT_TP_FNT_LFCLIPPRE				L"fnt_lfClipPrecision"
#define JSON_ENT_TP_FNT_LFQUALITY				L"fnt_lfQuality"
#define JSON_ENT_TP_FNT_LFPANDF					L"fnt_lfPitchAndFamily"
#define JSON_ENT_TP_FNT_LFFACENAME				L"fnt_lfFaceName"

#define JSON_ENT_TP_PAPERCOUNT					L"paper_count"
#define JSON_ENT_TP_TITLE						L"title"

#define JSON_ENT_TP_PAPERSEL					L"paper_selection"
#define JSON_ENT_TP_PAPERSRC					L"paper_source"
#define JSON_ENT_TP_PAPERTYPE					L"paper_type"
#define JSON_ENT_TP_PAPERSIZE					L"paper_size"
#define JSON_ENT_TP_ROTATE						L"rotate_180"
#define JSON_ENT_TP_SHIFTPIXEL					L"shift_pixel"
#define JSON_ENT_TP_PRINTTYPE					L"print_type"
#define JSON_ENT_TP_UNICODE						L"unicode"
#define JSON_ENT_TP_OUTPUTUNUSE					L"output_unuse"
#define JSON_ENT_TP_SHIFT						L"shift"
#define JSON_ENT_TP_PRINTTYPE					L"print_type"
#define JSON_ENT_TP_ORDER						L"order"
#define JSON_ENT_TP_POS1ST						L"position_1st"
#define JSON_ENT_TP_LENGTH						L"length"
#define JSON_ENT_TP_INTERVAL					L"interval"
#define JSON_ENT_TP_WIDTH						L"width"
#define JSON_ENT_TP_UNIT						L"unit"

#define JSON_KEY_TSF_ROOT_BASE					L"printer_ui\\tab_shift"
#define JSON_KEY_TSF_ROOT_BASE_US				L"printer_ui\\user_setting\\%ld\\tab_shift"
#define JSON_ENT_TSF_UNIT						L"unit"

// ================================================
// _/_/_/  Carbon Copy setting
// ================================================
#define JSON_KEY_CCP_ROOT_BASE					L"printer_ui\\carbon_copy"
#define JSON_KEY_CCP_INDEX_BASE					L"printer_ui\\carbon_copy\\%ld"
#define JSON_KEY_CCP_INDEX_INDEX_BASE			L"printer_ui\\carbon_copy\\%ld\\%ld"

#define JSON_ENT_CCP_COUNT						L"count"

#define JSON_ENT_CCP_BITFIELD					L"field"
#define JSON_ENT_CCP_PAGE						L"page"
#define JSON_ENT_CCP_PAPERSEL					L"paper_selection"
#define JSON_ENT_CCP_PAPERSRC					L"paper_source"
#define JSON_ENT_CCP_PAPERTYPE					L"paper_type"
#define JSON_ENT_CCP_PAPERSIZE					L"paper_size"
#define JSON_ENT_CCP_METHOD						L"printing_method"

#define JSON_ENT_CCP_PAPERCOUNT					L"paper_count"
#define JSON_ENT_CCP_TITLE						L"title"

// ================================================
// _/_/_/  stored to setting
// ================================================
#define JSON_KEY_STRD_ROOT_BASE					L"printer_ui\\job_control\\stored_to"
#define JSON_KEY_STRD_INDEX_BASE				L"printer_ui\\job_control\\stored_to\\%ld"

#define JSON_KEY_STRD_ROOT_BASE_US				L"printer_ui\\user_setting\\%ld\\job_control\\stored_to"

#define JSON_ENT_STRD_COUNT						L"count"
#define JSON_ENT_STRD_LASTSEL					L"last_selected"
#define JSON_ENT_STRD_LASTSELPIN				L"last_selected_pin"
#define JSON_ENT_STRD_LASTSELINDEX				L"last_selected_index"

#define JSON_ENT_STRD_FOLDERNAME				L"folder_name"
#define JSON_ENT_STRD_HASPIN					L"has_folder_pin"

// ================================================
// _/_/_/  Job Control setting
// ================================================
#define JSON_KEY_JC_ROOT_BASE					L"printer_ui\\job_control"

#define JSON_ENT_JC_ALWAYSPIN					L"use_PIN"
#define JSON_ENT_JC_ALWAYSACNUM					L"use_account_number"
#define JSON_ENT_JC_ALWAYSUNAME					L"use_user_name"
#define JSON_ENT_JC_ALWAYSJNAME					L"use_job_name"
#define JSON_ENT_JC_SETLNAME					L"set_login_name"
#define JSON_ENT_JC_SETLPASS					L"set_login_pass"
#define JSON_ENT_JC_FOLDERINDEX					L"folder_index"
#define JSON_ENT_JC_SETSINGLESIGNON				L"set_single_sign_on"

#define JSON_ENT_JC_USERNAME					L"user_name"
#define JSON_ENT_JC_JOBNAME						L"job_name"
#define JSON_ENT_JC_PIN							L"pin"
#define JSON_ENT_JC_PIN_SIZE					L"pin_size"
#define JSON_ENT_JC_ACNUM						L"account_number"
#define JSON_ENT_JC_ACNUM_SIZE					L"account_number_size"
#define JSON_ENT_JC_LOGINNAME					L"login_name"
#define JSON_ENT_JC_LOGINPASS					L"login_pass"
#define JSON_ENT_JC_LOGINPASS_SIZE				L"login_pass_size"
#define JSON_ENT_JC_FOLDERPASS					L"folder_pass"
#define JSON_ENT_JC_FOLDERPASS_SIZE				L"folder_pass_size"

// ================================================
// _/_/_/  ICC Profile setting
// ================================================
#define JSON_KEY_ICC_ROOT_BASE					L"printer_ui\\icc_profile"
#define JSON_KEY_ICC_INDEX_BASE					L"printer_ui\\icc_profile\\%ld"

#define JSON_ENT_ICC_COUNT						L"count"
#define JSON_ENT_ICC_PREICMSRCDEV				L"wPreIcmSrcDev"
#define JSON_ENT_ICC_PRENGRAYBMP				L"wPreNGrayBmp"
#define JSON_ENT_ICC_PRENGRAYGRP				L"wPreNGrayGrp"
#define JSON_ENT_ICC_PRENGRAYTXT				L"wPreNGrayTxt"
#define JSON_ENT_ICC_WVERSION					L"wVersion"
#define JSON_ENT_ICC_WSETTINGID					L"wSettingID"
#define JSON_ENT_ICC_WICMMETHOD					L"wIcmMethod"
#define JSON_ENT_ICC_WSRCPROFID					L"wSrcProfID"
#define JSON_ENT_ICC_WOUTPROFID					L"wOutProfID"
#define JSON_ENT_ICC_WRENDERID					L"wRenderID"
#define JSON_ENT_ICC_WRENDERVAL					L"wRenderVal"
#define JSON_ENT_ICC_WCMYKSIM					L"wCMYKSim"
#define JSON_ENT_ICC_WCMYKSIMPROF				L"wCMYKSimProf"
#define JSON_ENT_ICC_WGRAYBMP					L"wGrayBmp"
#define JSON_ENT_ICC_WGRAYGRP					L"wGrayGrp"
#define JSON_ENT_ICC_WGRAYTXT					L"wGrayTxt"
#define JSON_ENT_ICC_WPUREBKPRN					L"wPureBkPrn"
#define JSON_ENT_ICC_WBKOVERPRN					L"wBkOverPrn"
#define JSON_ENT_ICC_WSCREENBMP					L"wScreenBmp"
#define JSON_ENT_ICC_WSCREENGRP					L"wScreenGrp"
#define JSON_ENT_ICC_WSCREENTXT					L"wScreenTxt"
#define JSON_ENT_ICC_WPRNMODE					L"wPrnMode"
#define JSON_ENT_ICC_SZINBMP					L"szInputBmp"
#define JSON_ENT_ICC_SZINGRP					L"szInputGrp"
#define JSON_ENT_ICC_SZINTXT					L"szInputTxt"
#define JSON_ENT_ICC_SZOUTBMP					L"szOutputBmp"
#define JSON_ENT_ICC_SZOUTGRP					L"szOutputGrp"
#define JSON_ENT_ICC_SZOUTTXT					L"szOutputTxt"
#define JSON_ENT_ICC_WTITLENO					L"wTitleNo"
#define JSON_ENT_ICC_WPRESERVEPURE				L"wPreservePure"
#define JSON_ENT_ICC_TITLE						L"szTitle"

// ================================================
// _/_/_/  watermark
// ================================================
#define JSON_KEY_WM_ROOT_BASE					L"printer_ui\\watermark"
#define JSON_KEY_WM_INDEX_BASE					L"printer_ui\\watermark\\%ld"

#define JSON_ENT_WM_COUNT						L"count"
#define JSON_ENT_WM_CUSTOM_COLOR				L"custom_color"
#define JSON_ENT_WM_ID							L"id"

#define JSON_ENT_WM_JOBINFO						L"jobinfo"
#define JSON_ENT_WM_JOBINFO_WDM					L"ji_wmd"

#define JSON_ENT_WM_TITLE						L"title"
#define JSON_ENT_WM_TIME						L"time"

#define JSON_ENT_WM_DEC_CIRCLE					L"dm_wm_dec_circle"
#define JSON_ENT_WM_DEC_SQUARE					L"dm_wm_dec_square"
#define JSON_ENT_WM_FIRSTPAGEONLY				L"dm_wm_firstpageonly"
#define JSON_ENT_WM_TRANSPARENT					L"dm_wm_transparent"
#define JSON_ENT_WM_OUTLINE						L"dm_wm_outline"
#define JSON_ENT_WM_TRANSPARENT2				L"dm_wm_transparent2"
#define JSON_ENT_WM_PILE						L"dm_wm_pile"

#define JSON_ENT_WM_COLORREF					L"colorRef"
#define JSON_ENT_WM_NGRAYSCALE					L"nGrayscale"
#define JSON_ENT_WM_DWPOSX						L"dwPosX"
#define JSON_ENT_WM_DWPOSY						L"dwPosY"
#define JSON_ENT_WM_NSIZE						L"nSize"
#define JSON_ENT_WM_NANGLE						L"nAngle"
#define JSON_ENT_WM_WBORDERSIZE					L"wBorderSize"
#define JSON_ENT_WM_SZMAINWM					L"szMainWM"
#define JSON_ENT_WM_FNT_LFHEIGHT				L"fnt_lfHeight"
#define JSON_ENT_WM_FNT_LFWIDTH					L"fnt_lfWidth"
#define JSON_ENT_WM_FNT_LFESC					L"fnt_lfEscapement"
#define JSON_ENT_WM_FNT_LFORI					L"fnt_lfOrientation"
#define JSON_ENT_WM_FNT_LFWEIGHT				L"fnt_lfWeight"
#define JSON_ENT_WM_FNT_LFITALIC				L"fnt_lfItalic"
#define JSON_ENT_WM_FNT_LFUNDERLINE				L"fnt_lfUnderline"
#define JSON_ENT_WM_FNT_LFSTRIKEOUT				L"fnt_lfStrikeOut"
#define JSON_ENT_WM_FNT_LFCHARSET				L"fnt_lfCharSet"
#define JSON_ENT_WM_FNT_LFOUTPRE				L"fnt_lfOutPrecision"
#define JSON_ENT_WM_FNT_LFCLIPPRE				L"fnt_lfClipPrecision"
#define JSON_ENT_WM_FNT_LFQUALITY				L"fnt_lfQuality"
#define JSON_ENT_WM_FNT_LFPANDF					L"fnt_lfPitchAndFamily"
#define JSON_ENT_WM_FNT_LFFACENAME				L"fnt_lfFaceName"
#define JSON_ENT_WM_SELF						L"self"

// ================================================
// _/_/_/  image stamp
// ================================================
#define JSON_KEY_IS_ROOT_BASE					L"printer_ui\\image_stamp"
#define JSON_KEY_IS_INDEX_BASE					L"printer_ui\\image_stamp\\%ld"

#define JSON_ENT_IS_COUNT						L"count"

#define JSON_ENT_IS_FIRSTPAGEONLY				L"dm_is_firstpageonly"
#define JSON_ENT_IS_TRANSPARENT					L"dm_is_transparent"
#define JSON_ENT_IS_DWPOSX						L"dwPosX"
#define JSON_ENT_IS_DWPOSY						L"dwPosY"
#define JSON_ENT_IS_NANGLE						L"nAngle"
#define JSON_ENT_IS_SZFILEPATH					L"szFilePath"
#define JSON_ENT_IS_NSCALE						L"nScale"
#define JSON_ENT_IS_SELF						L"self"

#define JSON_ENT_IS_TITLE						L"title"
#define JSON_ENT_IS_TIME						L"time"

// ================================================
// _/_/_/  Woven Pattern Print Setting
// ================================================
#define JSON_KEY_WVN_ROOT_BASE					L"printer_ui\\woven_pattern"
#define JSON_KEY_WVN_INDEX_BASE					L"printer_ui\\woven_pattern\\%ld"

#define JSON_ENT_WVN_COUNT						L"count"

#define JSON_ENT_WVN_TITLE						L"title"
#define JSON_ENT_WVN_TIME						L"time"

#define JSON_ENT_WVN_EMBEDDED					L"embedded"
#define JSON_ENT_WVN_FIXEDTEXTLIST_INDEX		L"fixed_text_list_index"
#define JSON_ENT_WVN_ARBITRARY_TEXT				L"arbitrary_text"
#define JSON_ENT_WVN_SIZE						L"size"
#define JSON_ENT_WVN_ANGLE						L"angle"
#define JSON_ENT_WVN_EFFECT						L"effect"
#define JSON_ENT_WVN_COVER						L"cover"
#define JSON_ENT_WVN_DENSITY					L"density"
#define JSON_ENT_WVN_COLOR						L"color"
#define JSON_ENT_WVN_SELF						L"self"

// ================================================
// _/_/_/  Separator setting
// ================================================
#define JSON_KEY_SPRT_ROOT_BASE					L"printer_ui\\separator"
#define JSON_KEY_SPRT_INDEX_BASE				L"printer_ui\\separator\\%ld"
#define JSON_KEY_SPRT_INDEX_INDEX_BASE			L"printer_ui\\separator\\%ld\\%ld"

#define JSON_ENT_SPRT_COUNT						L"count"

#define JSON_ENT_SPRT_BITFIELD					L"field"
#define JSON_ENT_SPRT_INSERTPOS					L"insert_position"
#define JSON_ENT_SPRT_PAPERSRC					L"paper_source"
#define JSON_ENT_SPRT_PAPERTYPE					L"paper_type"
#define JSON_ENT_SPRT_TEXT						L"text"
#define JSON_ENT_SPRT_SELF						L"self"

#define JSON_ENT_SPRT_PAPERCOUNT				L"paper_count"
#define JSON_ENT_SPRT_TITLE						L"title"
#define JSON_ENT_SPRT_TIME						L"time"


// ================================================
// _/_/_/  overlay
// ================================================
#define JSON_KEY_OL_ROOT_BASE					L"printer_ui\\overlay"
#define JSON_KEY_OL_INDEX_BASE					L"printer_ui\\overlay\\%ld"

#define JSON_ENT_OL_COUNT						L"count"
#define JSON_ENT_OL_C_TITLE						L"create_title"
#define JSON_ENT_OL_C_PATH						L"create_path"
#define JSON_ENT_OL_C_STATE						L"create_state"
#define JSON_ENT_OL_LASTPATH					L"last_path"
#define JSON_ENT_OL_PIN_POS						L"pageinterleave_pos"

#define JSON_ENT_OL_TITLE						L"title"
#define JSON_ENT_OL_PATH						L"path"


#endif	// #ifndef _JSONKEY_H

