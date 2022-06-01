#include <iostream>
#include <Windows.h>
#include <atlstr.h>

using namespace std;

LARGE_INTEGER freq = { 0 };
LARGE_INTEGER cur_tick = { 0 };
LARGE_INTEGER prev_tick = { 0 };
LARGE_INTEGER diff_tick = { 0 };
LONGLONG elapsed_us = 0;

void TestCreateProcess()
{
    LPWSTR cmd = CA2T("EmptyProgram.exe");

    QueryPerformanceCounter(&prev_tick);

    for (int i = 0; i < 10000; i++)
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        si.cb = sizeof(STARTUPINFO);

        CreateProcess(NULL, cmd, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    QueryPerformanceCounter(&cur_tick);
    diff_tick.QuadPart = cur_tick.QuadPart - prev_tick.QuadPart;
    diff_tick.QuadPart *= 1000000;
    diff_tick.QuadPart /= freq.QuadPart;

    elapsed_us = diff_tick.QuadPart;

    cout << "[GJK] CreateProcess 10000 times uses " << elapsed_us << " us / " << (elapsed_us / 1000) << " ms / " << (elapsed_us / 1000000) << " s." << endl;
}

void TestOpenProcess()
{
    int pid;
    cout << "Insert Pid:";
    cin >> pid;
    cout << endl;

    QueryPerformanceCounter(&prev_tick);

    for (int i = 0; i < 10000; i++)
    {
        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        CloseHandle(handle);
    }

    QueryPerformanceCounter(&cur_tick);
    diff_tick.QuadPart = cur_tick.QuadPart - prev_tick.QuadPart;
    diff_tick.QuadPart *= 1000000;
    diff_tick.QuadPart /= freq.QuadPart;

    elapsed_us = diff_tick.QuadPart;

    cout << "[GJK] OpenProcess 10000 times uses " << elapsed_us << " us / " << (elapsed_us / 1000) << " ms / " << (elapsed_us / 1000000) << " s." << endl;
}

void TestFile()
{
    QueryPerformanceCounter(&prev_tick);

    for (int i = 0; i < 10000; i++)
    {
        CreateFile(
            L"TestFile.txt",
            GENERIC_ALL,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
    }

    QueryPerformanceCounter(&cur_tick);
    diff_tick.QuadPart = cur_tick.QuadPart - prev_tick.QuadPart;
    diff_tick.QuadPart *= 1000000;
    diff_tick.QuadPart /= freq.QuadPart;

    elapsed_us = diff_tick.QuadPart;

    cout << "[GJK] CreateFileA 10000 times uses " << elapsed_us << " us / " << (elapsed_us / 1000) << " ms / " << (elapsed_us / 1000000) << " s." << endl;
}

void TestOpenReg()
{
    HKEY predefinedKeys = HKEY_LOCAL_MACHINE;
    LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
    LPCTSTR lpValue = _T("CurrentBuildNumber");

    // 打开注册表项
    HKEY hKey;

    QueryPerformanceCounter(&prev_tick);

    for (int i = 0; i < 10000; i++)
    {
        RegOpenKeyEx(predefinedKeys, lpSubKey, NULL, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
    }

    QueryPerformanceCounter(&cur_tick);
    diff_tick.QuadPart = cur_tick.QuadPart - prev_tick.QuadPart;
    diff_tick.QuadPart *= 1000000;
    diff_tick.QuadPart /= freq.QuadPart;

    elapsed_us = diff_tick.QuadPart;

    cout << "[GJK] RegOpenKeyEx 10000 times uses " << elapsed_us << " us / " << (elapsed_us / 1000) << " ms / " << (elapsed_us / 1000000) << " s." << endl;

    // 获取返回数据的大小
    DWORD dataSize = 0;
    DWORD dwType = 0;
    DWORD dwFlags = RRF_RT_REG_SZ;
    RegGetValue(hKey, NULL, lpValue, dwFlags, &dwType, nullptr, &dataSize);

    // 创建符合返回数据大小的 CString 实例
    CString keyValue;
    const DWORD bufferLength = dataSize / sizeof(TCHAR);
    const TCHAR* textBuffer = keyValue.GetBuffer(bufferLength);

    QueryPerformanceCounter(&prev_tick);

    // 读取注册表数据
    for (int i = 0; i < 10000; i++)
    {
        RegGetValue(hKey, nullptr, lpValue, dwFlags, nullptr, (PVOID)textBuffer, &dataSize);
    }

    QueryPerformanceCounter(&cur_tick);
    diff_tick.QuadPart = cur_tick.QuadPart - prev_tick.QuadPart;
    diff_tick.QuadPart *= 1000000;
    diff_tick.QuadPart /= freq.QuadPart;

    elapsed_us = diff_tick.QuadPart;

    cout << "[GJK] RegGetValue 10000 times uses " << elapsed_us << " us / " << (elapsed_us / 1000) << " ms / " << (elapsed_us / 1000000) << " s." << endl;
}


int main()
{
    QueryPerformanceFrequency(&freq);

    TestCreateProcess();
    TestOpenProcess();
    TestOpenReg();
    TestFile();

    system("pause");
}

