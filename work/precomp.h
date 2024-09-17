//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : precomp.h
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 18 June 2013
//
//	Description   : Header files that should be in the precompiled header.

//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================

#pragma once

// Necessary for compiling under VC.
#if(!defined(WINVER) || (WINVER < 0x0500))
	#undef WINVER
	#define WINVER          0x0500
#endif
#if(!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0500))
	#undef _WIN32_WINNT
	#define _WIN32_WINNT    0x0500
#endif

// Isolation define for using ComCtrl v6.
#ifndef ISOLATION_AWARE_ENABLED
    #define ISOLATION_AWARE_ENABLED 
#endif
// Required header files that shouldn't change often.


#include <STDDEF.H>
#include <STDLIB.H>
#include <OBJBASE.H>
#include <STDARG.H>
#include <STDIO.H>
#include <INITGUID.H>
#include <commctrl.h>
#include "ActivationContext.h"
#include <WINDOWS.H>

#include <WINDEF.H>
#include <WINERROR.H>
#include <WINBASE.H>
#include <WINGDI.H>
#include <msxml6.h>

#include <WINSPOOL.H>
#include <TCHAR.H>
#include <EXCPT.H>
#include <ASSERT.H>
#include <PRSHT.H>
#include <COMPSTUI.H>
#include <WINDDIUI.H>
#include <PRINTOEM.H>
#include <prcomoem.h>
#include <string>
#include "xdexcept.h"
#include "xdstring.h"
#include <vector>
#include <map>
#include <list>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <msxml6.h>
#include "wtsapi32.h"

#include "resource.h"
#include "..\Include\PrinterUIResource.h"
#include "..\..\PrinterUIDialogResource\Include\PrinterUIDialogResource.h"
#include "scdmex.h"
#include "C_DMODE.H"
#include "devmode.h"
#include "c_windef.h"
#include "inifile.h"
#include "WaterMarkStructs.h"
#include "scuidata.h"
#include "inikey.h"
#include "WaterMarkRegistryData.h"
//#include "shJsonwm.h"
//#include "shjson.h"
#include "WaterMarkFile.h"
#include "UtilityStructure.h"
#include"PrintCoreHelper.h"
#include "UserData.h" 
#include "debug.h"
#include "SmartPtr.h"
#include "GPDFeatures.h"
#include "MilliInchPixelConversion.h"
#include "XMLDOMParser.h"
#include "ppstate.h"
#include "Utils.h"
#include "GDLSnapShot.h"
#include "GPDData.h"
#include "StringResource.h"
#include "ShowMessage.h"
#include "UIControl.h"
#include "StaticText.h"
// <S><A> To add bitmaps to custom controls like combox with image etc., 20200128 - SSDI:Seetharam
#include "Bitmap.h"
#include "ShBitmapList.h"
#include "Combo.h"
#include "ShImageCombo.h"
#include "ShBtnImgTxt.h" 
// <S><A> To add bitmaps to custom controls like combox with image etc., 20200128 - SSDI:Seetharam
#include "lim.h"
#include "Radio.h"
#include "CheckBox.h"
#include "CustomDialogData.h"
#include "AboutDialog.h"
#include "sh_dlg_printingpolicy.h"
#include "DefaultEdit.h"
#include "DefaultButton.h"
#include "Button.h"
#include "CopiesEdit.h"
#include "SpinControl.h"
#include "UserPaperSizeRegistry.h"
#include "ShBtnSpin.h"
#include "ShEditSpin.h"
//<S><A> To Add Support of Printing Position Dialoge, 03-06-2020, SSDI:Chanchal Parkash
#include "ShEditUnit.h"
#include "ShEditPPOS.h"
#include "ShMarginShiftRegistry.h"
#include "ShPrintPositionRegistry.h"
#include "ShCmbMarginShift.h"
#include "ShPrintPositionDialog.h"
#include "ShBtnMarginShift.h"
#include "ShComboBookletStandardPaperSize.h"
#include "ShComboColors.h"
#include "sh_cmb_userset.h"

#include "ShListUserSetting.h"
#include "ShListUserSetting_Com.h"
#include "ShUserSetDialog.h"
#include "ShJobControlRegistry.h"
#include "ShowMessageStaple.h"
#include "DialogCheckMessage.h"
#include "ShButtonOwnd.h"
#include "dlg_jobhand01.h"
#include "ShAuthenticationDialog.h"
//<E> To Add Support of Printing Position Dialoge, 03-06-2020, SSDI:Chanchal Parkash
#include "shListview.h"
#include "shListviewStored.h"
#include "CShDlgStored2.h"
#include "sh_dlg_addfolder.h"
#include "sh_edit_foldername.h"
#include "sh_edit.h"
#include "shregstr.h"
#include "sh_api.h"
#include "JobControlEdit.h"
#include "ShEditLoginName.h"
//<S><A>To Implement Task#3137,25-07-2024,SSDI:Manoj S
#include "ShEditJName.h"
//<E>To Implement Task #3137,25-07-2024,SSDI:Manoj S
#include "WaterMarkDialog.h"
#include "WaterMarkCombo.h"
#include "WaterMarkScrollbar.h"
#include "WaterMarkEdit.h"
#include "WaterMarkCheckBox.h"
#include "WaterMarkPreview.h"
#include "MimicImage.h"
#include "MimicText.h"
#include "MimicImageConfigTab.h"
#include "ColorPreview.h"
#include "InformationIconButton.h"
#include "CustomPaperEditBox.h"
#include "CustomPaperSettingsCombo.h"
#include "CustomPaperUnitRadio.h"
#include "SysLinkCtrl.h"
#include "DevModeUpDate.h"
#include "ShowJobHandlingMessage.h"
//<S><A> Added tab frame functionality, 20200327 - SSDI:Seetharam
#include "tabinfoctrl.h"
#include "ShTabdlgFrame.h"
//<S><E> Added tab frame functionality, 20200327 - SSDI:Seetharam
#include "DocumentPropertyPage.h"
#include "MainTabPropertyPage.h"
// <S><A> To add finishing tab in UD3L, 20200214 - SSDI:Goutham
#include "FinishingTabPropertyPage.h"
// <E> To add finishing tab in UD3L, 20200214 - SSDI:Goutham
#include "PaperTabPropertyPage.h"
// <S><A> To add layout tab in UD3L, 20200214 - SSDI:Goutham
#include "LayoutTabPropertyPage.h"
// <E> To add layout tab in UD3L, 20200214 - SSDI:Goutham
#include "WaterMarkTabPropertyPage.h"
#include "JobHandlingTabPropertyPage.h"
#include "ColorTabPropertyPage.h"
#include "DevicePropertyPage.h"
#include "intrface.h"
#include "CustomPaperDlg.h"
#include "CustomPaperSettingsDlg.h"
#include "CustomFormEditBox.h"
#include "WaterMarkAddDialog.h"
// <S><A> To add Other Settings Dialog in UD3L, 20200409 - SSDI:Goutham
#include "OtherSettingsDialog.h"
// <E> To add Other Settings Dialog in UD3L, 20200409 - SSDI:Goutham
#include "JobHandlingDialog.h"
#include "Core.h"
#include "RegUtil.h"



#include "PrinterEvent.h"
#include  "CustomPaperButton.h"
#include "LangCombo.h"
#include "PrinterSelectionCombo.h"
#include "PrinterSelectionButton.h"
#include "DocumentEvent.h"
//#include <intsafe.h>

// Include this file after any other standard include files 
// to disallow unsafe string handling functions.
#include <STRSAFE.H>
// <S><A> To add Zoom Settings Dialog in UD3L, 20200408 - SSDI:Sneha TG
#include "ZoomSettingsDialog.h"
// <S><E> To add Zoom Settings Dialog in UD3L, 20200408 - SSDI:Sneha TG

#include "sh_dlg_update.h"
#include "sh_dlg_autoconf.h"