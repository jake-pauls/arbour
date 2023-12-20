#pragma once

class Win32Application
{
public:
	Win32Application() = default;
	~Win32Application() = default;

	/// @brief Starts the Win32Application's runtime.
	/// @param hInstance Handle for the currently running instance
	/// @param nCmdShow Display code for the window being spawned
	i32 Run(HINSTANCE hInstance, i32 nCmdShow) const;

	/// @brief Retrieves the static window handle saved for this application.
	FORCEINLINE static constexpr const HWND& GetWindowHandle() 
	{ 
		return m_WindowHandle; 
	};

private:
	static HWND m_WindowHandle;
};