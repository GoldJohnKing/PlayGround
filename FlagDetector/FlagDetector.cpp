// FlagDetector.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <vector>
#include <utility>
#include <iostream>
#include <Windows.h>

int main()
{
    //DWORD dwExStyle = 0x14C00000L; // “雷火”
    //DWORD dwExStyle = 0x56000000L; // 启动器
    DWORD dwExStyle = 0x9400004CL; // “温瑞安逆水寒”
    system("pause");
    std::vector<std::pair<DWORD, std::string>> flagList = {
        {WS_EX_ACCEPTFILES, "WS_EX_ACCEPTFILES"},
        {WS_EX_APPWINDOW, "WS_EX_APPWINDOW"},
        {WS_EX_CLIENTEDGE, "WS_EX_CLIENTEDGE"},
        {WS_EX_COMPOSITED, "WS_EX_COMPOSITED"},
        {WS_EX_CONTEXTHELP, "WS_EX_CONTEXTHELP"},
        {WS_EX_CONTROLPARENT, "WS_EX_CONTROLPARENT"},
        {WS_EX_DLGMODALFRAME, "WS_EX_DLGMODALFRAME"},
        {WS_EX_LAYERED, "WS_EX_LAYERED"},
        {WS_EX_LAYOUTRTL, "WS_EX_LAYOUTRTL"},
        {WS_EX_LEFT, "WS_EX_LEFT"},
        {WS_EX_LEFTSCROLLBAR, "WS_EX_LEFTSCROLLBAR"},
        {WS_EX_LTRREADING, "WS_EX_LTRREADING"},
        {WS_EX_MDICHILD, "WS_EX_MDICHILD"},
        {WS_EX_NOACTIVATE, "WS_EX_NOACTIVATE"},
        {WS_EX_NOINHERITLAYOUT, "WS_EX_NOINHERITLAYOUT"},
        {WS_EX_NOPARENTNOTIFY, "WS_EX_NOPARENTNOTIFY"},
        {WS_EX_NOREDIRECTIONBITMAP, "WS_EX_NOREDIRECTIONBITMAP"},
        {WS_EX_OVERLAPPEDWINDOW, "WS_EX_OVERLAPPEDWINDOW"},
        {WS_EX_PALETTEWINDOW, "WS_EX_PALETTEWINDOW"},
        {WS_EX_RIGHT, "WS_EX_RIGHT"},
        {WS_EX_RIGHTSCROLLBAR, "WS_EX_RIGHTSCROLLBAR"},
        {WS_EX_RTLREADING, "WS_EX_RTLREADING"},
        {WS_EX_STATICEDGE, "WS_EX_STATICEDGE"},
        {WS_EX_TOOLWINDOW, "WS_EX_TOOLWINDOW"},
        {WS_EX_TOPMOST, "WS_EX_TOPMOST"},
        {WS_EX_TRANSPARENT, "WS_EX_TRANSPARENT"},
        {WS_EX_WINDOWEDGE, "WS_EX_WINDOWEDGE"}
    };

    std::cout << "Modified!" << std::endl;

    for (std::pair<DWORD, std::string> flag: flagList) {
        if ((dwExStyle & flag.first) == flag.first)
            std::cout << flag.second << std::endl;
    }

}
