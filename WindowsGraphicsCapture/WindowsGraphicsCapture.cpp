// WindowsGraphicsCapture.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>

#include <dxgi.h>
#include <d3d11.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <Unknwn.h>
#include <inspectable.h>

#include <wil/cppwinrt.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <Windows.Graphics.DirectX.Direct3d11.interop.h>

using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;

int main()
{

}

static struct winrt_capture* winrt_capture_init_internal(BOOL cursor,
	HWND window,
	BOOL client_area,
	HMONITOR monitor)
	try {
	ID3D11Device* const d3d_device = (ID3D11Device*)gs_get_device_obj();
	ComPtr<IDXGIDevice> dxgi_device;

	HRESULT hr = d3d_device->QueryInterface(&dxgi_device);
	if (FAILED(hr)) {
		blog(LOG_ERROR, "Failed to get DXGI device");
		return nullptr;
	}

	winrt::com_ptr<IInspectable> inspectable;
	hr = CreateDirect3D11DeviceFromDXGIDevice(dxgi_device.Get(),
		inspectable.put());
	if (FAILED(hr)) {
		blog(LOG_ERROR, "Failed to get WinRT device");
		return nullptr;
	}

	auto activation_factory = winrt::get_activation_factory<
		winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
	auto interop_factory =
		activation_factory.as<IGraphicsCaptureItemInterop>();
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem item =
		winrt_capture_create_item(interop_factory.get(), window,
			monitor);
	if (!item)
		return nullptr;

	const winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice
		device = inspectable.as<winrt::Windows::Graphics::DirectX::
		Direct3D11::IDirect3DDevice>();
	const winrt::Windows::Graphics::SizeInt32 size = item.Size();
	const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool
		frame_pool = winrt::Windows::Graphics::Capture::
		Direct3D11CaptureFramePool::Create(
			device,
			winrt::Windows::Graphics::DirectX::
			DirectXPixelFormat::B8G8R8A8UIntNormalized,
			2, size);
	const winrt::Windows::Graphics::Capture::GraphicsCaptureSession session =
		frame_pool.CreateCaptureSession(item);

	if (winrt_capture_border_toggle_supported()) {
		winrt::Windows::Graphics::Capture::GraphicsCaptureAccess::
			RequestAccessAsync(
				winrt::Windows::Graphics::Capture::
				GraphicsCaptureAccessKind::Borderless)
			.get();
		session.IsBorderRequired(false);
	}

	/* disable cursor capture if possible since ours performs better */
	const BOOL cursor_toggle_supported =
		winrt_capture_cursor_toggle_supported();
	if (cursor_toggle_supported)
		session.IsCursorCaptureEnabled(cursor);

	struct winrt_capture* capture = new winrt_capture{};
	capture->window = window;
	capture->client_area = client_area;
	capture->monitor = monitor;
	capture->capture_cursor = cursor && cursor_toggle_supported;
	capture->cursor_visible = cursor;
	capture->item = item;
	capture->device = device;
	d3d_device->GetImmediateContext(&capture->context);
	capture->frame_pool = frame_pool;
	capture->session = session;
	capture->last_size = size;
	capture->closed = item.Closed(winrt::auto_revoke,
		{ capture, &winrt_capture::on_closed });
	capture->frame_arrived = frame_pool.FrameArrived(
		winrt::auto_revoke,
		{ capture, &winrt_capture::on_frame_arrived });
	capture->next = capture_list;
	capture_list = capture;

	session.StartCapture();
	capture->active = TRUE;

	gs_device_loss callbacks;
	callbacks.device_loss_release = winrt_capture_device_loss_release;
	callbacks.device_loss_rebuild = winrt_capture_device_loss_rebuild;
	callbacks.data = capture;
	gs_register_loss_callbacks(&callbacks);

	return capture;

}
catch (const winrt::hresult_error& err) {
	blog(LOG_ERROR, "winrt_capture_init (0x%08X): %ls", err.code().value,
		err.message().c_str());
	return nullptr;
}
catch (...) {
	blog(LOG_ERROR, "winrt_capture_init (0x%08X)",
		winrt::to_hresult().value);
	return nullptr;
}
