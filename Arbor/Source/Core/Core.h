#pragma once

#include <memory>

#ifdef WIN32
#include <wrl/client.h>
#endif

namespace Core
{
/// @brief Alias for a `std::unique_ptr` with a given type.
/// @tparam Type Type associated with the unique pointer
template<typename Type>
using UniquePtr = std::unique_ptr<Type>;

/// @brief Alias for `std::make_unique` that forwards all variadic arguments.
/// @tparam Type Type associated with the unique pointer
/// @tparam ...ArgTypes Types for the variadic arguments forwarded into the pointer's construction
/// @param ...args Values for the variadic arguments forwarded into the pointer's construction
/// @return Constructed unique pointer
template<typename Type, typename... ArgTypes>
constexpr UniquePtr<Type> MakeUnique(ArgTypes... args)
{
	return std::make_unique<Type>(std::forward<ArgTypes>(args)...);
}

/// @brief Alias for a `std::shared_ptr` with a given type.
/// @tparam Type Type associated with the shared pointer
template<typename Type>
using SharedPtr = std::shared_ptr<Type>;

/// @brief Alias for `std::make_shared` that forwards all variadic arguments.
/// @tparam Type Type associated with the shared pointer
/// @tparam ...ArgTypes Types for the variadic arguments forwarded into the pointer's construction
/// @param ...args Values for the variadic arguments forwarded into the pointer's construction
/// @return Constructed shared pointer
template<typename Type, typename... ArgTypes>
constexpr SharedPtr<Type> MakeShared(ArgTypes... args)
{
	return std::make_shared<Type>(std::forward<ArgTypes>(args)...);
}

#ifdef WIN32
/// @brief Alias for the use of `Microsoft::WRL::ComPtr`
/// @tparam Type Templated type argument for the ComPtr
/// @note For more information on the use and recommendation of ComPtr in UWP/Win32 applications, 
///       refer to: https://github.com/Microsoft/DirectXTK/wiki/ComPtr
template<typename Type>
using ComPtr = Microsoft::WRL::ComPtr<Type>;
#endif
}
