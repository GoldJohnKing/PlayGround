#pragma once
//#include "IFBCInterface.h"

// Windows Implementation Library
#include <wil/cppwinrt.h>

// WinRT
#include <winrt/Windows.System.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>

// Win32
#include <d3d11_1.h>
#include <Windows.h>

class WindowsGraphicsCapture/* : public IGrabObjectEx*/
{
public:
    WindowsGraphicsCapture();
    ~WindowsGraphicsCapture();

    // Public APIs inherited from base class
    //virtual void SetFBCGlobalStatus(IFBCGlobalStatus* pIn);
    //virtual void SetEncoderManager(IFBCEncodeManager* pIn);
    virtual BOOL InitGrabObject();
    virtual BOOL StartFBCGrab();
    virtual BOOL StopFBCGrab();
    //virtual BOOL SnapShot(const videoShareParam& param, int nWidth, int nHeight);
    virtual void Release();

    // Public APIs of current class
    void SetCaptureItem(HWND hWnd);
    void SetCaptureItem(HMONITOR hMonitor);

private:
    //IFBCGlobalStatus* m_pGlobalStatus = nullptr;
    //IFBCEncodeManager* m_pEncodeManager = nullptr;

    // WinRT Apartment
    bool InitializeWinRTApartment();
    void UninitializeWinRTApartment();

    // ApiInformation
    bool IsWindowsGraphicsCaptureSupportedByCurrentOsVersion();

    // DispatcherQueueController
    winrt::Windows::System::DispatcherQueueController m_dispatcherQueueController{ nullptr };

    bool CreateDispatcherQueueController();
    void CloseDispatcherQueueController();

    // Direct3D
    winrt::com_ptr<ID3D11Device> m_d3d11Device;
    winrt::com_ptr<ID3D11DeviceContext> m_d3d11DeviceContext;
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_direct3dDevice{ nullptr };

    bool CreateWinRTCompatibleD3D11Devices();
    void CloseWinRTCompatibleD3D11Devices();

    // GraphicsCaptureItem
    HWND m_hWnd = NULL;
    HMONITOR m_hMonitor = NULL;
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_graphicsCaptureItem{ nullptr };

    bool CreateGraphicsCaptureItem();
    bool CreateGraphicsCaptureItem(HWND hWnd);
    bool CreateGraphicsCaptureItem(HMONITOR hMonitor);
    void CloseGraphicsCaptureItem();

    // Direct3D11CaptureFramePool, GraphicsCaptureSession
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_direct3d11CaptureFramePool{ nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_graphicsCaptureSession{ nullptr };
    winrt::Windows::Graphics::SizeInt32 m_contentSize{};

    bool CreateDirect3D11CaptureFramePoolAndGraphicsCaptureSession();
    bool CloseDirect3D11CaptureFramePoolAndGraphicsCaptureSession();
    bool GraphicsCapruteSessionStart();

    // FrameArrived
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker m_frameArrived{};
    bool InvokeFrameArrivedEvent();
    void RevokeFrameArrivedEvent();
    void OnFrameArrived(winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender, winrt::Windows::Foundation::IInspectable const&);

};
