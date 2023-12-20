#pragma once

class Win32Window 
{
public:
	Win32Window() = default;
	~Win32Window() = default;

	/// @brief Initializes the Win32Window and shows it on screen.
	/// @param hInstance Handle for the currently running instance
	/// @param nCmdShow Display code for the window being spawned
	/// @param title title for the Win32 window
	void Init(HINSTANCE hInstance, i32 nCmdShow, const wchar_t* title = TEXT("Win32 Application"));

	/// @brief Peeks the next message in the command queue.
	/// @param msg Reference to a message to peek, translate, and dispatch
	void Peek(LPMSG msg) const;

	/// @brief Destroys the active window.
	void Destroy();
 
	/// @brief Retrieves the Win32 handle for this window.
	FORCEINLINE constexpr const HWND& GetHWND() 
	{ 
		return m_Hwnd; 
	}

protected:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_Hwnd;
};