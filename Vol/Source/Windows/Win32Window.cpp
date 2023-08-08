#include "Win32Window.h"

HWND Win32Window::Hwnd{ nullptr };

namespace Win32WindowPrivate
{
// Handle for the currently running instance, this shouldn't be used anywhere else
HINSTANCE HInstance{ nullptr };

// Window class name used for registration purposes
constexpr wchar_t const* WindowClassName = TEXT("VolDX12");
}

void Win32Window::Init(HINSTANCE HInstance, int NCmdShow, const wchar_t* Title) const
{
	Win32WindowPrivate::HInstance = HInstance;

	WNDCLASS WindowClass = {};
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = HInstance;
	WindowClass.lpszClassName = Win32WindowPrivate::WindowClassName;

	RegisterClass(&WindowClass);

	RECT WindowRect = { 0, 0, static_cast<LONG>(CoreStatics::ViewportWidth), static_cast<LONG>(CoreStatics::ViewportHeight) };
	AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

	Hwnd = CreateWindowEx(0, 
		Win32WindowPrivate::WindowClassName, 
		Title, 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		WindowRect.right - WindowRect.left, 
		WindowRect.bottom - WindowRect.top,
		NULL,
		NULL,
		HInstance,
		NULL);

	is(Hwnd);
	ShowWindow(Hwnd, NCmdShow);
}

void Win32Window::Peek(LPMSG Msg) const
{
	if (PeekMessage(Msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(Msg);
		DispatchMessage(Msg);
	}
}

void Win32Window::Destroy() const
{
	DestroyWindow(Hwnd);
	Hwnd = nullptr;

	UnregisterClass(Win32WindowPrivate::WindowClassName, Win32WindowPrivate::HInstance);
	Win32WindowPrivate::HInstance = nullptr;
}

LRESULT CALLBACK Win32Window::WindowProc(HWND Hwnd, UINT Message, WPARAM WParam, LPARAM LParam)
{
	is(Hwnd);

	switch (Message)
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
		return DefWindowProc(Hwnd, Message, WParam, LParam);
	}

	return 0;
}
