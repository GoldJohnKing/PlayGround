// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

LRESULT CALLBACK MyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (WM_KILLFOCUS == uMsg)
        return FALSE;
}

LONG_PTR WINAPI MySetWindowLong(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    if (GWLP_WNDPROC != nIndex)
        return dwNewLong;
    else
        return (LONG_PTR)MyWindowProc;
}

LONG_PTR WINAPI MySetWindowLongPtrA(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    return MySetWindowLong(hWnd, nIndex, dwNewLong);
}

LONG_PTR WINAPI MySetWindowLongPtrW( HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    return MySetWindowLong(hWnd, nIndex, dwNewLong);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    LONG error;

    HMODULE user32 = GetModuleHandle(_T("user32.dll"));
    if (NULL == user32) {
        OutputDebugString(_T("GetModuleHandle: Load User32.dll Failed."));
        return FALSE;
    }

    PVOID setWindowLongPtrA = GetProcAddress(user32,		"SetWindowLongPtrA");
    PVOID setWindowLongPtrW = GetProcAddress(user32,		"SetWindowLongPtrW");

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        OutputDebugString(_T("DLL_PROCESS_ATTACH: Started."));

        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&setWindowLongPtrA, MySetWindowLongPtrA);
        DetourAttach(&setWindowLongPtrW, MySetWindowLongPtrW);
        error = DetourTransactionCommit();

        if (error == NO_ERROR)
            OutputDebugString(_T("DLL_PROCESS_ATTACH: Succeed."));
        else
            OutputDebugString(_T("DLL_PROCESS_ATTACH: Failed."));
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        OutputDebugString(_T("DLL_PROCESS_DETACH: Started."));

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&setWindowLongPtrA, MySetWindowLongPtrA);
        DetourDetach(&setWindowLongPtrW, MySetWindowLongPtrW);
        error = DetourTransactionCommit();

        if (error == NO_ERROR)
            OutputDebugString(_T("DLL_PROCESS_DETACH: Succeed."));
        else
            OutputDebugString(_T("DLL_PROCESS_DETACH: Failed."));
    }

    return TRUE;
}

