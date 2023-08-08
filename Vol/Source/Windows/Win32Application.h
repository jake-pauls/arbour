#pragma once

class Win32Application
{
public:
	Win32Application() = default;
	~Win32Application() = default;

	/**
	 * @brief Starts the Win32Application's runtime.
	 * @param HInstance Handle for the currently running instance
	 * @param NCmdShow Display code for the window being spawned
	 */
	int Run(HINSTANCE HInstance, int NCmdShow) const;
};