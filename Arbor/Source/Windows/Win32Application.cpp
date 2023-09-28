#include "Win32Application.h"

#include "D3D12/D3D12Renderer.h"
#include "Win32Window.h"

HWND Win32Application::m_WindowHandle;

int Win32Application::Run(HINSTANCE hInstance, int nCmdShow) const
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

	window->Destroy();
	window = nullptr;

	renderer->Destroy();
	renderer = nullptr;

	return static_cast<int>(Msg.wParam);
}
