// CreateProcessCmd.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <atlstr.h>
#include <string>

int main()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);

    CString cmd = "c://windows//system32//cmd.exe /c C:\\Windows\\System32\\nvidia-smi.exe";

    CreateProcess(NULL, cmd.GetBuffer(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

std::string RunCmd(const char* cmd)
{
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead, hWrite;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        printf("error");
    }
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    std::string runcmd(cmd);
    runcmd = "c://windows//system32//cmd.exe /c " + runcmd;

    if (!CreateProcess(NULL, (LPSTR)runcmd.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        printf("error");
    }
    CloseHandle(hWrite);

    char buffer[2048] = { 0 };
    std::string result;

    DWORD bytesRead;

    while (1) {
        if (ReadFile(hRead, buffer, 2048, &bytesRead, NULL) == FALSE)
            break;
        result = result + buffer;
        memset(buffer, 0, 2048);
        //sleep(1);
    }
    return result;

}

int main2() {
    std::string ret = RunCmd("C:\\Windows\\System32\\nvidia-smi.exe");
    std::cout << ret << std::endl;
    system("pause");
}
