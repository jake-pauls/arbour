#include "D3D12Renderer.h"

#include "D3D12Helpers.h"
#include "Windows/Win32Application.h"

#ifdef WIN32
#include <DXGIDebug.h>
#endif

namespace D3D12RendererPrivate
{
// Dedicated frames to await
// todo: will other D3D12 structures require this data?
static constexpr uint32_t FrameCount{ 2 };
}

void D3D12Renderer::Init()
{
	// todo: Abstract pipelines into a dedicated DeviceResources class (similar to VkHardware in Vulkan)
	InitPipelines();
}

void D3D12Renderer::Render() const
{
}

void D3D12Renderer::Destroy() const
{
#ifdef ARBOR_DEBUG
	// Use the stored debug information to check for 'leaks' reported by Windows
	{
		Core::ComPtr<IDXGIDebug1> DxgiDebug;
		if (DX_CALL_SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DxgiDebug))))
		{
			DxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
}

void D3D12Renderer::InitPipelines()
{
	uint32_t DxgiFactoryFlags = 0;

#ifdef ARBOR_DEBUG
	// Enable debug mode for D3D12
	{
		ID3D12Debug* DebugController;
		if (DX_CALL_SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
		{
			DebugController->EnableDebugLayer();
			DxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		Core::ComPtr<IDXGIInfoQueue> DxgiInfoQueue;
		if (DX_CALL_SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(DxgiInfoQueue.GetAddressOf()))))
		{
			DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
	}
#endif

	// Factory
	IDXGIFactory4* Factory;
	DX_CALL(CreateDXGIFactory2(DxgiFactoryFlags, IID_PPV_ARGS(&Factory)));

	// Device
	// todo: add warp adapter? 
	// https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp
	{
		IDXGIAdapter1* HardwareAdapter;
		GetHardwareAdapter(Factory, &HardwareAdapter);
		DX_CALL(D3D12CreateDevice(HardwareAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device)));
		is(Device);
	}

	// Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
		CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&CommandQueue));
		is(CommandQueue);
	}

	// Swap Chain
	{
		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
		SwapChainDesc.BufferCount = D3D12RendererPrivate::FrameCount;
		SwapChainDesc.Width = CoreStatics::ViewportWidth;
		SwapChainDesc.Height = CoreStatics::ViewportHeight;
		SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.SampleDesc.Count = 1;

		Core::ComPtr<IDXGISwapChain1> LocalSwapChain;
		// Creating a swap chain flushes the command queue
		DX_CALL(Factory->CreateSwapChainForHwnd(CommandQueue.Get(), Win32Application::GetWindowHandle(), &SwapChainDesc, nullptr, nullptr, &LocalSwapChain));

		// Disable use of Alt+Enter for fullscreen transitions
		DX_CALL(Factory->MakeWindowAssociation(Win32Application::GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER));

		// todo: why do we reassign this to the heap one, instead of just allocating it there in the first place?
		DX_CALL(LocalSwapChain.As(&SwapChain));
		FrameIndex = SwapChain->GetCurrentBackBufferIndex();
	}

	// Descriptor Heaps
	{
		// Describe and create a Render Target View descriptor heap
		// todo: look into different descriptor heap types
		D3D12_DESCRIPTOR_HEAP_DESC RtvDescriptorHeapDesc = {};
		RtvDescriptorHeapDesc.NumDescriptors = D3D12RendererPrivate::FrameCount;
		RtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		DX_CALL(Device->CreateDescriptorHeap(&RtvDescriptorHeapDesc, IID_PPV_ARGS(&RtvDescriptorHeap)));
		RtvDescriptorHeapSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// todo: add submodule for directx12 helpers before continuing
	// refer to: https://github.com/microsoft/DirectX-Headers/blob/main/README.md
}

void D3D12Renderer::GetHardwareAdapter(IDXGIFactory1* Factory, IDXGIAdapter1** Adapter, bool bRequestHighPerformanceAdapter) const
{
	*Adapter = nullptr;

	IDXGIAdapter1* Adapter1 = nullptr;
	IDXGIFactory6* Factory6 = nullptr;
	
	if (DX_CALL_SUCCEEDED(Factory->QueryInterface(IID_PPV_ARGS(&Factory6))))
	{
		for (uint32_t Index = 0; 
			DX_CALL_SUCCEEDED(Factory6->EnumAdapterByGpuPreference(Index,
				bRequestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&Adapter1))); 
			++Index)
		{
			// todo: put into lambda?
			DXGI_ADAPTER_DESC1 Desc;
			Adapter1->GetDesc1(&Desc);

			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// To use a software adapter, look into WARP.
				continue;
			}

			// Check to see if the device supports Direct3D 12
			if (DX_CALL_SUCCEEDED(D3D12CreateDevice(Adapter1, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	if (!Adapter1)
	{
		for (uint32_t Index = 0; DX_CALL_SUCCEEDED(Factory->EnumAdapters1(Index, &Adapter1)); ++Index)
		{
			DXGI_ADAPTER_DESC1 Desc;
			Adapter1->GetDesc1(&Desc);

			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// To use a software adapter, look into WARP.
				continue;
			}

			// Check to see if the device supports Direct3D 12
			if (DX_CALL_SUCCEEDED(D3D12CreateDevice(Adapter1, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*Adapter = Adapter1;
}
