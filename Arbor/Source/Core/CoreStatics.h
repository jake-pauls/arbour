#pragma once

#include "CoreTypes.h"

namespace CoreStatics
{
// Starting width of the viewport
constexpr static u16 ViewportWidth{ 800 };

// Starting height of the viewport
constexpr static u16 ViewportHeight{ 600 };

// Starting aspect ratio of the viewport
constexpr static float ViewportAspectRatio{ static_cast<float>(ViewportWidth) / static_cast<float>(ViewportHeight) };
}