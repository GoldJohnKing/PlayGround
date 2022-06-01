// WindowsVersionDetector.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <atlstr.h>
#include <string>

int GetWindowsBuildVersion()
{
	HKEY predefinedKeys = HKEY_LOCAL_MACHINE;
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	LPCTSTR lpValue = _T("CurrentBuildNumber");

	// 打开注册表项
	HKEY hKey;
	LSTATUS returnCode = RegOpenKeyEx(predefinedKeys, lpSubKey, NULL, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (ERROR_SUCCESS != returnCode)
		return 0;

	// 获取返回数据的大小
	DWORD dataSize = 0;
	DWORD dwType = 0;
	DWORD dwFlags = RRF_RT_REG_SZ;
	returnCode = RegGetValue(hKey, NULL, lpValue, dwFlags, &dwType, nullptr, &dataSize);
	if (ERROR_SUCCESS != returnCode || 0 == dataSize || REG_SZ != dwType)
		return 0;

	// 创建符合返回数据大小的 CString 实例
	CString keyValue;
	const DWORD bufferLength = dataSize / sizeof(TCHAR);
	const TCHAR* textBuffer = keyValue.GetBuffer(bufferLength);

	// 读取注册表数据
	returnCode = RegGetValue(hKey, nullptr, lpValue, dwFlags, nullptr, (PVOID)textBuffer, &dataSize);
	if (ERROR_SUCCESS != returnCode)
		return 0;

	// 删除 CString 字符串末尾的结束符
	keyValue.ReleaseBuffer();

	return stoi(std::wstring(keyValue));
}

int main()
{
	if(GetWindowsBuildVersion() >= 20000)
		std::cout << "Good";
	else
		std::cout << "Bad";
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
