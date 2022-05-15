#include "WindowsGraphicsCapture.h"

// Win32 COM
#include <Unknwn.h>
#include <inspectable.h>

// Windows Implementation Library
#include <wil/cppwinrt.h>
#include <wil/resource.h>

// WinRT - Windows Runtime Library
#include <wrl/client.h>

// WinRT
// We shall always include header files recursively
#include <winrt/Windows.Foundation.h>
//#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>

// Interops
// CreateDirect3D11DeviceFromDXGIDevice, CreateDirect3D11SurfaceFromDXGISurface
#include <windows.graphics.directx.direct3d11.interop.h>
// IGraphicsCaptureItemInterop
#include <windows.graphics.capture.interop.h>

// DispatcherQueueController
#include <dispatcherqueue.h>

// Win32
#include <dxgi.h>
#include <d3d11_1.h>
#include <Windows.h>


//
// Public APIs inherited from base class
//

WindowsGraphicsCapture::WindowsGraphicsCapture()
{
    //InitializeWinRTApartment();
}

WindowsGraphicsCapture::~WindowsGraphicsCapture()
{
    //Release();

    //UninitializeWinRTApartment();
}

//void WindowsGraphicsCapture::SetFBCGlobalStatus(IFBCGlobalStatus* pIn)
//{
//    m_pGlobalStatus = pIn;
//}
//
//void WindowsGraphicsCapture::SetEncoderManager(IFBCEncodeManager* pIn)
//{
//    m_pEncodeManager = pIn;
//}

BOOL WindowsGraphicsCapture::InitGrabObject()
{
    return
        InitializeWinRTApartment() &&

        IsWindowsGraphicsCaptureSupportedByCurrentOsVersion() &&

        CreateDispatcherQueueController() &&

        CreateWinRTCompatibleD3D11Devices();
}

BOOL WindowsGraphicsCapture::StartFBCGrab()
{
    return
        CreateGraphicsCaptureItem() &&

        CreateDirect3D11CaptureFramePoolAndGraphicsCaptureSession() &&

        InvokeFrameArrivedEvent() &&

        GraphicsCapruteSessionStart();
}

BOOL WindowsGraphicsCapture::StopFBCGrab()
{
    RevokeFrameArrivedEvent();

    return true;
}

//BOOL WindowsGraphicsCapture::SnapShot(const videoShareParam& param, int nWidth, int nHeight)
//{
//    return 0;
//}

void WindowsGraphicsCapture::Release()
{
    CloseDirect3D11CaptureFramePoolAndGraphicsCaptureSession();

    CloseGraphicsCaptureItem();

    CloseWinRTCompatibleD3D11Devices();

    delete this;
}


//
// Public APIs of current class
//

void WindowsGraphicsCapture::SetCaptureItem(HWND hWnd)
{
    m_hWnd = hWnd;
    m_hMonitor = NULL;
}

void WindowsGraphicsCapture::SetCaptureItem(HMONITOR hMonitor)
{
    m_hMonitor = hMonitor;
    m_hWnd = NULL;
}


//
// Private functions of current class
//

bool WindowsGraphicsCapture::InitializeWinRTApartment()
{
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    }
    catch (...)
    {
        return false;
    }


    return true;
}

void WindowsGraphicsCapture::UninitializeWinRTApartment()
{
    winrt::uninit_apartment();
}

bool WindowsGraphicsCapture::IsWindowsGraphicsCaptureSupportedByCurrentOsVersion()
{
    if (!winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 9))
    {
        return false;
    }

    return true;
}

bool WindowsGraphicsCapture::CreateDispatcherQueueController()
{
    DispatcherQueueOptions options{ sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_STA };

    HRESULT hr = ::CreateDispatcherQueueController(options,
        reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(winrt::put_abi(m_dispatcherQueueController)));

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void WindowsGraphicsCapture::CloseDispatcherQueueController()
{
    m_dispatcherQueueController = nullptr;
}

bool WindowsGraphicsCapture::CreateWinRTCompatibleD3D11Devices()
{
    D3D_FEATURE_LEVEL d3d_feature_level[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        d3d_feature_level, sizeof(d3d_feature_level) / sizeof(D3D_FEATURE_LEVEL),
        D3D11_SDK_VERSION, m_d3d11Device.put(), nullptr, m_d3d11DeviceContext.put());

    if (FAILED(hr))
    {
        return false;
    }

    winrt::com_ptr<IDXGIDevice> dxgi_device = m_d3d11Device.as<IDXGIDevice>();

    winrt::com_ptr<IInspectable> inspectable;
    hr = CreateDirect3D11DeviceFromDXGIDevice(dxgi_device.get(), inspectable.put());

    if (FAILED(hr))
    {
        return false;
    }

    m_direct3dDevice = inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();

    m_d3d11Device->GetImmediateContext(m_d3d11DeviceContext.put());

    return true;
}

void WindowsGraphicsCapture::CloseWinRTCompatibleD3D11Devices()
{
    m_direct3dDevice = nullptr;
    m_d3d11DeviceContext = nullptr;
}

bool WindowsGraphicsCapture::CreateGraphicsCaptureItem()
{
    bool ret = false;

    if (m_hWnd)
        ret = CreateGraphicsCaptureItem(m_hWnd);
    else if (m_hMonitor)
        ret = CreateGraphicsCaptureItem(m_hMonitor);
    else
    {
        return false;
    }

    if (!ret)
    {
        return false;
    }

    return true;
}

bool WindowsGraphicsCapture::CreateGraphicsCaptureItem(HWND hWnd)
{
    winrt::impl::com_ref<IGraphicsCaptureItemInterop>
        interop_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();

    HRESULT hr = interop_factory->CreateForWindow(hWnd, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(),
        reinterpret_cast<void**>(winrt::put_abi(m_graphicsCaptureItem)));

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool WindowsGraphicsCapture::CreateGraphicsCaptureItem(HMONITOR hMonitor)
{
    winrt::impl::com_ref<IGraphicsCaptureItemInterop>
        interop_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();

    HRESULT hr = interop_factory->CreateForMonitor(hMonitor, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(),
        reinterpret_cast<void**>(winrt::put_abi(m_graphicsCaptureItem)));

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void WindowsGraphicsCapture::CloseGraphicsCaptureItem()
{
    m_graphicsCaptureItem = nullptr;
}

bool WindowsGraphicsCapture::CreateDirect3D11CaptureFramePoolAndGraphicsCaptureSession()
{
    try {
        m_direct3d11CaptureFramePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded(m_direct3dDevice,
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized, 1, m_graphicsCaptureItem.Size());
    }
    catch (winrt::hresult_error& err)
    {
        return false;
    }

    try {
        m_graphicsCaptureSession = m_direct3d11CaptureFramePool.CreateCaptureSession(m_graphicsCaptureItem);
    }
    catch (winrt::hresult_error& err)
    {
        return false;
    }

    m_graphicsCaptureSession.IsCursorCaptureEnabled(false);

    return true;
}

bool WindowsGraphicsCapture::CloseDirect3D11CaptureFramePoolAndGraphicsCaptureSession()
{
    bool ret = true;

    try {
        m_direct3d11CaptureFramePool.Close();
    }
    catch (winrt::hresult_error& err)
    {
        ret = false;
    }

    try {
        m_graphicsCaptureSession.Close();
    }
    catch (winrt::hresult_error& err)
    {
        ret = false;
    }

    // As we are going to release all the resources, we should always set Direct3D11CaptureFramePool and GraphicsCaptureSession to nullptr,
    // regardless of whether they are successfully closed or not
    m_direct3d11CaptureFramePool = nullptr;
    m_graphicsCaptureSession = nullptr;

    return ret;
}

bool WindowsGraphicsCapture::GraphicsCapruteSessionStart()
{
    try {
        m_graphicsCaptureSession.StartCapture();
    }
    catch (winrt::hresult_error& err)
    {
        return false;
    }

    return true;
}

bool WindowsGraphicsCapture::InvokeFrameArrivedEvent()
{
    try {
        /*m_frameArrived = */m_direct3d11CaptureFramePool.FrameArrived(/*winrt::auto_revoke, */{ this, &WindowsGraphicsCapture::OnFrameArrived });
    }
    catch (winrt::hresult_error& err)
    {
        return false;
    }

    return true;
}

void WindowsGraphicsCapture::RevokeFrameArrivedEvent()
{
    m_frameArrived.revoke();
}

struct __declspec(uuid("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1")) IDirect3DDxgiInterfaceAccess : ::IUnknown {
    virtual HRESULT __stdcall GetInterface(GUID const& id, void** object) = 0;
};

template<typename T>
winrt::com_ptr<T> GetDXGIInterfaceFromObject(winrt::Windows::Foundation::IInspectable const& object)
{
    auto access = object.as<IDirect3DDxgiInterfaceAccess>();
    winrt::com_ptr<T> result;
    winrt::check_hresult(access->GetInterface(winrt::guid_of<T>(), result.put_void()));
    return result;
}

void WindowsGraphicsCapture::OnFrameArrived(winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender, winrt::Windows::Foundation::IInspectable const&)
{
    OutputDebugStringA("GJK OnFrameArrived");

    winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame direct3d11CaptureFrame{ nullptr };

    try {
        direct3d11CaptureFrame = sender.TryGetNextFrame();
    }
    catch (winrt::hresult_error& err)
    {
        return;
    }

    winrt::Windows::Graphics::SizeInt32 contentSize{};

    try {
        contentSize = direct3d11CaptureFrame.ContentSize();
    }
    catch (winrt::hresult_error& err)
    {
    }

    winrt::com_ptr<ID3D11Texture2D> d3d11Texture2D;

    try {
        d3d11Texture2D = GetDXGIInterfaceFromObject<ID3D11Texture2D>(direct3d11CaptureFrame.Surface());
    }
    catch (winrt::hresult_error& err)
    {
        return;
    }

    //    stGrabResult m_grabResult{};
    //m_grabResult._nGrabWidth = contentSize.Width;
    //m_grabResult._nGrabHeight = contentSize.Height;
    //m_grabResult._pGrabAcquireTexture = d3d11Texture2D.get();
    //m_grabResult._pGrabDevice = m_d3d11Device.get();
    //m_grabResult.dwCapStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(direct3d11CaptureFrame.SystemRelativeTime()).count();
    //m_grabResult._CurrentFormat = 0;
    //m_grabResult._pGrabTexture[0] = d3d11Texture2D.get();

    //    winrt::com_ptr<IDXGIResource> dxgi_resource = d3d11Texture2D.as<IDXGIResource>();
    //HRESULT hr = dxgi_resource.get()->GetSharedHandle(&m_grabResult._hSharedHandle[0]);

    //m_d3d11DeviceContext->CopyResource((ID3D11Texture2D*)texture, d3d11Texture2D.get());

    //m_pEncodeManager->FBC_Present(m_grabResult);


    if (contentSize.Width != m_contentSize.Width || contentSize.Height != m_contentSize.Height)
    {

        try {
            m_direct3d11CaptureFramePool.Recreate(m_direct3dDevice,
                winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized, 1, contentSize);

            m_contentSize = contentSize;
        }
        catch (winrt::hresult_error& err)
        {
        }
    }
    OutputDebugStringA("GJK OnFrameArrived Done");
}
