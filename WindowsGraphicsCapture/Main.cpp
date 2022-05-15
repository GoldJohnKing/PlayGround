// WindowsGraphicsCapture.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "WindowsGraphicsCapture.h"

#include <Windows.h>

#include <dxgi.h>
#include <d3d11.h>

#include <thread>

std::unique_ptr<std::thread>  wgcCaptureThread;

void WgcCaptureThread()
{
    WindowsGraphicsCapture* wgc = new WindowsGraphicsCapture();
    HMONITOR hMonitor = MonitorFromPoint({ 1248,750 }, MONITOR_DEFAULTTOPRIMARY);
    //HMONITOR hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
    wgc->SetCaptureItem(hMonitor);
    wgc->InitGrabObject();
    wgc->StartFBCGrab();
}

int main()
{
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    wgcCaptureThread.reset(new std::thread(&WgcCaptureThread));

    Sleep(3600000);

    return 0;
}
