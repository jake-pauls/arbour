#pragma once

// Win32
#if WIN32
#include <Windows.h>
#include <D3D12.h>
#include <DXGI1_6.h>
#include <D3DCompiler.h>
#include <DirectXMath.h>
#endif

// Core
// todo: Abstract these out to a single CorePCH file
#include "Core/Core.h"
#include "Core/CoreStatics.h"
#include "Core/CoreTypes.h"
#include "Core/Defines.h"
