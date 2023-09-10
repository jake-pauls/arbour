#include "Win32Application.h"

#include "D3D12/D3D12Renderer.h"
#include "Win32Window.h"

HWND Win32Application::WindowHandle;

int Win32Application::Run(HINSTANCE HInstance, int NCmdShow) const
{
	Core::UniquePtr<Win32Window> Window = Core::MakeUnique<Win32Window>();
	Core::UniquePtr<D3D12Renderer> Renderer = Core::MakeUnique<D3D12Renderer>();

	Window->Init(HInstance, NCmdShow, TEXT("arbor - DirectX12"));
	WindowHandle = Window->GetHWND();

	Renderer->Init();

	MSG Msg = {};
	while (Msg.message != WM_QUIT)
	{
		Window->Peek(&Msg);
		Renderer->Render();
	}

	Window->Destroy();
	Window = nullptr;

	Renderer->Destroy();
	Renderer = nullptr;

	return static_cast<int>(Msg.wParam);
}
