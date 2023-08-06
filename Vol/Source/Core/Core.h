#pragma once

#include <memory>

namespace Core
{
/**
 * @brief Alias for a `std::unique_ptr` with a given type.
 * @tparam Type Type associated with the unique pointer
 */
template<typename Type>
using UniquePtr = std::unique_ptr<Type>;

/**
 * @brief Alias for `std::make_unique` that forwards all variadic arguments.
 * @tparam Type Type associated with the unique pointer
 * @tparam ...Args Types for the variadic arguments forwarded into the pointer's construction
 * @param ...args Values for the variadic arguments forwarded into the pointer's construction
 * @return Constructed unique pointer
 */
template<typename Type, typename... Args>
constexpr UniquePtr<Type> MakeUnique(Args... args)
{
	return std::make_unique<Type>(std::forward<Args>(args)...);
}

/**
 * @brief Alias for a `std::shared_ptr` with a given type.
 * @tparam Type Type associated with the shared pointer
 */
template<typename Type>
using SharedPtr = std::shared_ptr<Type>;

/**
 * @brief Alias for `std::make_shared` that forwards all variadic arguments.
 * @tparam Type Type associated with the shared pointer
 * @tparam ...Args Types for the variadic arguments forwarded into the pointer's construction
 * @param ...args Values for the variadic arguments forwarded into the pointer's construction
 * @return Constructed shared pointer
 */
template<typename Type, typename... Args>
constexpr SharedPtr<Type> MakeShared(Args... args)
{
	return std::make_shared<Type>(std::forward<Args>(args)...);
}
}
