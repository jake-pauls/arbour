#pragma once

#include <filesystem>
#include <iomanip>
#include <iostream>

namespace DefinesPrivate
{
#ifdef VOL_DEBUG 
#ifdef WIN32
// Size of the buffer allocated to print into Visual Studio's output window
constexpr int VsPrintSize{ 256 };
#endif

// Label to indicate the debug logging level
constexpr char const* LogLevelDebug{ "DEBUG" };
// Label to indicate the warning logging level
constexpr char const* LogLevelWarning{ "WARNING" };
// Label to indicate the error logging level
constexpr char const* LogLevelError{ "ERROR" };
#endif
}

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define __NEWLINE__ "\n"
#define __FILE_NAME__ std::filesystem::path(__FILE__).filename().string()
#define __FILE_NAME_C_STR__ std::filesystem::path(__FILE__).filename().string().c_str()

#ifdef VOL_DEBUG
	#ifdef WIN32
		#define DEBUGBREAK() __debugbreak()
	#else
		#include <signal.h>
		#define DEBUGBREAK() raise(SIGTRAP)
	#endif
#else
	#define DEBUGBREAK()
#endif

#ifdef VOL_DEBUG
	#ifdef WIN32
		#include <assert.h>
	#else
		#include <cassert>
	#endif
#endif

#ifdef VOL_DEBUG
	#ifdef WIN32
		// todo: find an alternate method with soft buffer maximum
		#define __VS_PRINT__(Fmt, ...) \
		{ \
			char Str[DefinesPrivate::VsPrintSize]; \
			sprintf_s(Str, sizeof(Str), Fmt, __VA_ARGS__); \
			OutputDebugStringA(Str); \
		} \

		#define __PRINT__(Fmt, ...) fprintf(stdout, Fmt, __VA_ARGS__); __VS_PRINT__(Fmt, __VA_ARGS__)
	#else
		#define __PRINT__(Fmt, ...) fprintf(stdout, Fmt, __VA_ARGS__); 
	#endif

	#define __LOG_FMT__ "[%-2s | %-2s | %s:%d] "
	#define __LOG_ARGS__(LOG_TAG) LOG_TAG, __FILE_NAME_C_STR__, __FUNCTION__, __LINE__
	#define __LOG__(Level, Message, ...) __PRINT__(__LOG_FMT__ Message __NEWLINE__, __LOG_ARGS__(Level), ## __VA_ARGS__)

	#define VOL_LOG(Message, ...) __LOG__(DefinesPrivate::LogLevelDebug, Message, __VA_ARGS__)
	#define VOL_WARNING(Message, ...) __LOG__(DefinesPrivate::LogLevelWarning, Message, __VA_ARGS__)
	#define VOL_ERROR(Message, ...) __LOG__(DefinesPrivate::LogLevelError, Message, __VA_ARGS__)
#else
	#define VOL_LOG(Message, ...) 
	#define VOL_ERROR(Message, ...)
#endif

#ifdef VOL_DEBUG
	/**
	 * Wrapper for a generic platform assert, causes a crashe if provided check is failed.
	 */
	#define is(Check) assert(Check)

	/**
	 * Cool trick inspired by how UE tackles boolean-ish assert checking.
	 * https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Core/Public/Misc/AssertionMacros.h
	 */
	#define __VERIFY_LAMBDA__(Capture, Check, Message, ...) \
		[Capture]() -> bool { \
			if (!(Check)) \
			{ \
				VOL_ERROR(Message, ## __VA_ARGS__); DEBUGBREAK(); \
				return false; \
			} \
			return true; \
		}() \

	/**
	 * Similar to `ensure` in UE, `verify` permits conditioning to check if an assert is true.
	 * 
	 *		if (verify(Object.BoolProperty))
	 *      {
	 *			// Do something with this object's bool property
	 *      }
	 *
	 * Unlike `is`, `verify` will execute a debug break in debug builds and continue execution of the running program.
	 */
	#define verify(Check) __VERIFY_LAMBDA__(, Check, #Check)
	/**
	 * @see verify(...)
	 */
	#define verifyf(Check, Message, ...) __VERIFY_LAMBDA__(&, Check, Message, ## __VA_ARGS__)
#else 
	#define verify(Check) 
	#define verifyf(Check, Message, ...)
#endif 
