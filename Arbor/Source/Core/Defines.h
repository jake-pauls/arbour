#pragma once

#include <filesystem>
#include <iomanip>
#include <iostream>

namespace DefinesPrivate
{
#ifdef ARBOR_DEBUG 
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

#ifdef ARBOR_DEBUG
	#ifdef WIN32
		#define DEBUGBREAK() __debugbreak()
	#else
		#include <signal.h>
		#define DEBUGBREAK() raise(SIGTRAP)
	#endif
#else
	#define DEBUGBREAK()
#endif

#ifdef ARBOR_DEBUG
	#ifdef WIN32
		#include <assert.h>
	#else
		#include <cassert>
	#endif
#endif

#ifdef ARBOR_DEBUG
	#ifdef WIN32
		// todo: find an alternate method with soft buffer maximum
		#define __VS_PRINT__(fmt, ...) \
		{ \
			char str[DefinesPrivate::VsPrintSize]; \
			sprintf_s(str, sizeof(str), fmt, __VA_ARGS__); \
			OutputDebugStringA(str); \
		} \

		#define __PRINT__(fmt, ...) fprintf(stdout, fmt, __VA_ARGS__); __VS_PRINT__(fmt, __VA_ARGS__)
	#else
		#define __PRINT__(fmt, ...) fprintf(stdout, fmt, __VA_ARGS__); 
	#endif

	#define __LOG_FMT__ "[%-2s | %-2s | %s:%d] "
	#define __LOG_ARGS__(LOG_TAG) LOG_TAG, __FILE_NAME_C_STR__, __FUNCTION__, __LINE__
	#define __LOG__(level, message, ...) __PRINT__(__LOG_FMT__ message __NEWLINE__, __LOG_ARGS__(level), ## __VA_ARGS__)

	#define ARBOR_LOG(message, ...) __LOG__(DefinesPrivate::LogLevelDebug, message, __VA_ARGS__)
	#define ARBOR_WARNING(message, ...) __LOG__(DefinesPrivate::LogLevelWarning, message, __VA_ARGS__)
	#define ARBOR_ERROR(message, ...) __LOG__(DefinesPrivate::LogLevelError, message, __VA_ARGS__)
#else
	#define ARBOR_LOG(message, ...) 
	#define ARBOR_ERROR(message, ...)
#endif

#ifdef ARBOR_DEBUG
	/// Wrapper for a generic platform assert, causes a crashe if provided check is failed.
	#define is(check) assert(check)

	/// Equivalent to `is(false)`, can be used to signify a code path that should have no entry
	#define isFatal() assert(false)

	/// Cool trick inspired by how UE tackles boolean-ish assert checking.
	/// https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Core/Public/Misc/AssertionMacros.h
	#define __VERIFY_LAMBDA__(capture, check, message, ...) \
		[capture]() -> bool { \
			if (!(check)) \
			{ \
				ARBOR_ERROR(message, ## __VA_ARGS__); DEBUGBREAK(); \
				return false; \
			} \
			return true; \
		}() \

	///
	/// Similar to `ensure` in UE, `verify` permits conditioning to check if an assert is true.
	/// 
	///		if (verify(Object.BoolProperty))
	///     {
	///			// Do something with this object's bool property
	///     }
	///
	/// Unlike `is`, `verify` will execute a debug break in debug builds and continue execution of the running program.
	///
	#define verify(check) __VERIFY_LAMBDA__(, check, #check)
	
	/// @see verify(...)
	#define verifyf(check, message, ...) __VERIFY_LAMBDA__(&, check, message, ## __VA_ARGS__)
#else 
	#define verify(check) 
	#define verifyf(check, message, ...)
#endif 
