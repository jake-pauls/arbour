#include "D3D12Renderer.h"

#include "D3D12Helpers.h"
#include "Windows/Win32Application.h"

#ifdef WIN32
#include <DXGIDebug.h>
#include <d3dx12.h>
#endif

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
		Core::ComPtr<IDXGIDebug1> dxgiDebug;
		if (DX_CALL_SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
}

void D3D12Renderer::InitPipelines()
{
	uint32_t dxgiFactoryFlags = 0;

#ifdef ARBOR_DEBUG
	// Enable debug mode for D3D12
	{
		ID3D12Debug* debugController;
		if (DX_CALL_SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		Core::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (DX_CALL_SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
	}
#endif

	// Factory
	IDXGIFactory4* factory;
	DX_CALL(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	// Device
	// todo: add warp adapter? 
	// https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp
	{
		IDXGIAdapter1* hardwareAdapter;
		GetHardwareAdapter(factory, &hardwareAdapter);
		DX_CALL(D3D12CreateDevice(hardwareAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
		is(m_Device);
	}

	// Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue));
		is(m_CommandQueue);
	}

	// Swap Chain
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_FrameCount;
		swapChainDesc.Width = CoreStatics::ViewportWidth;
		swapChainDesc.Height = CoreStatics::ViewportHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		Core::ComPtr<IDXGISwapChain1> localSwapChain;
		// Creating a swap chain flushes the command queue
		DX_CALL(factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), Win32Application::GetWindowHandle(), &swapChainDesc, nullptr, nullptr, &localSwapChain));

		// Disable use of Alt+Enter for fullscreen transitions
		DX_CALL(factory->MakeWindowAssociation(Win32Application::GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER));

		// todo: why do we reassign this to the heap one, instead of just allocating it there in the first place?
		DX_CALL(localSwapChain.As(&m_SwapChain));
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}

	// Descriptor Heaps
	{
		// Describe and create a Render Target View descriptor heap
		// todo: look into different descriptor heap types
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
		rtvDescriptorHeapDesc.NumDescriptors = m_FrameCount;
		rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		DX_CALL(m_Device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&m_RtvDescriptorHeap)));
		m_RtvDescriptorHeapSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// todo: add submodule for directx12 helpers before continuing
	// refer to: https://github.com/microsoft/DirectX-Headers/blob/main/README.md

	// Frame Resources
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// Create an RTV for each frame
		for (uint32_t i = 0; i < m_FrameCount; ++i)
		{
			DX_CALL(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));
			m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_RtvDescriptorHeapSize);
		}
	}

	DX_CALL(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));
}

void D3D12Renderer::InitAssets()
{
}

void D3D12Renderer::GetHardwareAdapter(IDXGIFactory1* Factory, IDXGIAdapter1** adapter, bool bRequestHighPerformanceAdapter) const
{
	*adapter = nullptr;

	IDXGIAdapter1* adapter1 = nullptr;
	IDXGIFactory6* factory6 = nullptr;
	
	if (DX_CALL_SUCCEEDED(Factory->QueryInterface(IID_PPV_ARGS(&factory6))))
	{
		for (uint32_t i = 0; 
			DX_CALL_SUCCEEDED(factory6->EnumAdapterByGpuPreference(i,
				bRequestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&adapter1))); 
			++i)
		{
			// todo: put into lambda?
			DXGI_ADAPTER_DESC1 Desc;
			adapter1->GetDesc1(&Desc);

			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// To use a software adapter, look into WARP.
				continue;
			}

			// Check to see if the device supports Direct3D 12
			if (DX_CALL_SUCCEEDED(D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	if (!adapter1)
	{
		for (uint32_t i = 0; DX_CALL_SUCCEEDED(Factory->EnumAdapters1(i, &adapter1)); ++i)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter1->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// To use a software adapter, look into WARP.
				continue;
			}

			// Check to see if the device supports Direct3D 12
			if (DX_CALL_SUCCEEDED(D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*adapter = adapter1;
}
