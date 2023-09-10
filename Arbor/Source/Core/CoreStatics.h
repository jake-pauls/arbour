#pragma once

namespace CoreStatics
{
// Starting width of the viewport
constexpr static uint16_t ViewportWidth{ 800 };

// Starting height of the viewport
constexpr static uint16_t ViewportHeight{ 600 };

// Starting aspect ratio of the viewport
constexpr static float ViewportAspectRatio { static_cast<float>(ViewportWidth) / static_cast<float>(ViewportHeight) };
}