#pragma once

#include "CopiesEdit.h"  // Include the base class header

class CCustomJobControlEdit : public CJobControlEdit
{
public:
    // Constructors
    CCustomJobControlEdit(
        WCHAR* szEditName,
        UINT uEditNameLength,
        BOOL blForceMB = FALSE
    );

    CCustomJobControlEdit(
        WCHAR* szEditName,
        UINT uEditNameLength,
        EEDITBOXTYPE eEditBoxType,
        BOOL blForceMB = FALSE
    );

    CCustomJobControlEdit(
        HMODULE hStringResourceHandle,
        HANDLE hPrinter,
        HWND PropertySheetHandle,
        short wEditID,
        WCHAR* szEditName,
        UINT uEditNameLength,
        EEDITBOXTYPE eEditBoxType,
        BOOL blForceMB = FALSE
    );

    CCustomJobControlEdit(
        WCHAR* szEditName,
        UINT uEditNameLength,
        SHORT wEditID,
        BOOL blForceMB = FALSE
    );

    // Override methods
    virtual LRESULT WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam) override;
    virtual void Attach(HWND hWnd) override;
};

LRESULT CALLBACK CustomGlobalWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
