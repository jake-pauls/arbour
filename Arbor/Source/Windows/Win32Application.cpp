#include "Win32Application.h"

#include "D3D12/D3D12Renderer.h"
#include "Win32Window.h"

HWND Win32Application::m_WindowHandle;

i32 Win32Application::Run(HINSTANCE hInstance, i32 nCmdShow) const
{
	Core::UniquePtr<Win32Window> window = Core::MakeUnique<Win32Window>();
	Core::UniquePtr<D3D12Renderer> renderer = Core::MakeUnique<D3D12Renderer>();

	window->Init(hInstance, nCmdShow, TEXT("arbor - DirectX12"));
	m_WindowHandle = window->GetHWND();

	renderer->Init();

	MSG Msg = {};
	while (Msg.message != WM_QUIT)
	{
		window->Peek(&Msg);
		renderer->Render();
	}

	renderer->Destroy();
	window->Destroy();

	return static_cast<i32>(Msg.wParam);
}
