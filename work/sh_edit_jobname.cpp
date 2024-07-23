#include "CustomJobControlEdit.h"

CCustomJobControlEdit::CCustomJobControlEdit(WCHAR* szEditName, UINT uEditNameLength, BOOL blForceMB)
    : CJobControlEdit(szEditName, uEditNameLength, blForceMB)
{
}

CCustomJobControlEdit::CCustomJobControlEdit(WCHAR* szEditName, UINT uEditNameLength, EEDITBOXTYPE eEditBoxType, BOOL blForceMB)
    : CJobControlEdit(szEditName, uEditNameLength, eEditBoxType, blForceMB)
{
}

CCustomJobControlEdit::CCustomJobControlEdit(HMODULE hStringResourceHandle, HANDLE hPrinter, HWND PropertySheetHandle, short wEditID, WCHAR* szEditName, UINT uEditNameLength, EEDITBOXTYPE eEditBoxType, BOOL blForceMB)
    : CJobControlEdit(szEditName, uEditNameLength, eEditBoxType, blForceMB)
{
}

CCustomJobControlEdit::CCustomJobControlEdit(WCHAR* szEditName, UINT uEditNameLength, SHORT wEditID, BOOL blForceMB)
    : CJobControlEdit(szEditName, uEditNameLength, wEditID, blForceMB)
{
}

LRESULT CCustomJobControlEdit::WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    if (!IsWindow(hWnd))
    {
        return TRUE;
    }

    switch (nMsg)
    {
    case WM_CHAR:
        {
            static const WCHAR FORBIDDEN_CHAR[] = L"<>:\"/\\|?*"; // Example forbidden characters
            for (int i = 0; FORBIDDEN_CHAR[i] != '\0'; ++i)
            {
                if (wParam == FORBIDDEN_CHAR[i])
                {
                    return 0;
                }
            }
            break;
        }
    default:
        break;
    }

    return CJobControlEdit::WndProc(hWnd, nMsg, wParam, lParam);  // Call the base class WndProc
}

void CCustomJobControlEdit::Attach(HWND hWnd)
{
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CustomGlobalWndProc));
}

LRESULT CALLBACK CustomGlobalWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    CCustomJobControlEdit* pThis = reinterpret_cast<CCustomJobControlEdit*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (pThis)
    {
        return pThis->WndProc(hWnd, nMsg, wParam, lParam);
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}
