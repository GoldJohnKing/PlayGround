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
