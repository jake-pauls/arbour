#pragma once

class Win32Window 
{
public:
	Win32Window() = default;
	~Win32Window() = default;

	/**
	 * @brief Initializes the Win32Window and shows it on screen.
	 * @param HInstance Handle for the currently running instance
	 * @param NCmdShow Display code for the window being spawned
	 * @param Title Title for the Win32 window
	 */
	void Init(HINSTANCE HInstance, int NCmdShow, const wchar_t* Title = TEXT("Win32 Application"));

	/**
	 * @brief Peeks the next message in the command queue.
	 * @param Msg Reference to a message to peek, translate, and dispatch
	 */
	void Peek(LPMSG Msg) const;

	/**
	 * @brief Destroys the active window.
	 */
	void Destroy();
 
	/**
	 * @brief Retrieves the Win32 handle for this window.
	 */
	FORCEINLINE constexpr const HWND& GetHWND() 
	{ 
		return Hwnd; 
	}

protected:
	static LRESULT CALLBACK WindowProc(HWND Hwnd, UINT Message, WPARAM WParam, LPARAM LParam);

private:
	HWND Hwnd;
};