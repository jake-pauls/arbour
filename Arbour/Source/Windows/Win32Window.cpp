#include "Win32Window.h"

namespace Win32WindowPrivate
{
// Handle for the currently running instance, this shouldn't be used anywhere else
HINSTANCE HInstance{ nullptr };

// Window class name used for registration purposes
constexpr wchar_t const* WindowClassName = TEXT("Arbour - DirectX12");
}	// Win32WindowPrivate

void Win32Window::Init(HINSTANCE hInstance, i32 nCmdShow, const wchar_t* title)
{
	Win32WindowPrivate::HInstance = hInstance;

	WNDCLASS windowClass = {};
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = Win32WindowPrivate::WindowClassName;

	RegisterClass(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(CoreStatics::ViewportWidth), static_cast<LONG>(CoreStatics::ViewportHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	m_Hwnd = CreateWindowEx(0, 
		Win32WindowPrivate::WindowClassName, 
		title, 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		windowRect.right - windowRect.left, 
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	is(m_Hwnd);
	ShowWindow(m_Hwnd, nCmdShow);
}

void Win32Window::Peek(LPMSG msg) const
{
	if (PeekMessage(msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(msg);
		DispatchMessage(msg);
	}
}

void Win32Window::Destroy()
{
	DestroyWindow(m_Hwnd);
	m_Hwnd = nullptr;

	UnregisterClass(Win32WindowPrivate::WindowClassName, Win32WindowPrivate::HInstance);
	Win32WindowPrivate::HInstance = nullptr;
}

LRESULT CALLBACK Win32Window::WindowProc(HWND m_Hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	is(m_Hwnd);

	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
		// Update
		break;
	case WM_DESTROY:
		[[fallthrough]];
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		// Handle other window events if we don't do anything
		return DefWindowProc(m_Hwnd, message, wParam, lParam);
	}

	return 0;
}
