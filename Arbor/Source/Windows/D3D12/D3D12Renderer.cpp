#include "D3D12Renderer.h"

#include "D3D12Helpers.h"
#include "Windows/Win32Application.h"

#include <DXGIDebug.h>
#include <d3dx12.h>
#include <iterator>

D3D12Renderer::D3D12Renderer()
	: m_Viewport(0.f, 0.f, static_cast<float>(CoreStatics::ViewportWidth), static_cast<float>(CoreStatics::ViewportHeight))
	, m_ScissorRect(0, 0, static_cast<long>(CoreStatics::ViewportWidth), static_cast<long>(CoreStatics::ViewportHeight))
{
}

void D3D12Renderer::Init()
{
	// todo: Abstract pipelines into a dedicated DeviceResources class (similar to VkHardware in Vulkan)
	InitPipelines();
	InitResources();
}

void D3D12Renderer::Render()
{
	PopulateCommandList();

	ID3D12CommandList* commandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(static_cast<u32>(std::size(commandLists)), commandLists);

	DX_CALL(m_SwapChain->Present(1, 0));

	WaitForPreviousFrame();
}

void D3D12Renderer::Destroy() 
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

	WaitForPreviousFrame();

	CloseHandle(m_FenceEvent);
}

void D3D12Renderer::InitPipelines()
{
	u32 dxgiFactoryFlags = 0;

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
	Core::ComPtr<IDXGIFactory4> factory;
	DX_CALL(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	// Device
	// todo: add warp adapter? 
	// https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp
	{
		Core::ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		DX_CALL(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
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

		Core::ComPtr<IDXGISwapChain1> swapChain;
		// Creating a swap chain flushes the command queue
		DX_CALL(factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), Win32Application::GetWindowHandle(), &swapChainDesc, nullptr, nullptr, &swapChain));

		// Disable use of Alt+Enter for fullscreen transitions
		DX_CALL(factory->MakeWindowAssociation(Win32Application::GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER));

		// todo: why do we reassign this to the heap one, instead of just allocating it there in the first place?
		DX_CALL(swapChain.As(&m_SwapChain));
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

	// Frame Resources
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// Create an RTV for each frame
		for (u32 index = 0; index < m_FrameCount; ++index)
		{
			DX_CALL(m_SwapChain->GetBuffer(index, IID_PPV_ARGS(&m_RenderTargets[index])));
			m_Device->CreateRenderTargetView(m_RenderTargets[index].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_RtvDescriptorHeapSize);
		}
	}

	DX_CALL(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));
}

void D3D12Renderer::InitResources()
{
	// Root Signature
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Core::ComPtr<ID3DBlob> signature;
		Core::ComPtr<ID3DBlob> error;
		DX_CALL(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		DX_CALL(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
	}

	// Pipeline State - also, compiling/loading shaders
	{
		Core::ComPtr<ID3DBlob> vertexShader;
		Core::ComPtr<ID3DBlob> pixelShader;

#ifdef ARBOR_DEBUG
		u32 compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		u32 compileFlags = 0;
#endif

		// todo: Write file helpers for pathing
		DX_CALL(D3DCompileFromFile(TEXT("Resources\\Shaders\\Shaders.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
		DX_CALL(D3DCompileFromFile(TEXT("Resources\\Shaders\\Shaders.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

		// Vertex input layout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		// Graphics Pipeline State Object (PSO)
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, static_cast<u32>(std::size(inputElementDescs)) };
		psoDesc.pRootSignature = m_RootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = false;
		psoDesc.DepthStencilState.StencilEnable = false;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		DX_CALL(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	// Command List
	DX_CALL(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), m_PipelineState.Get(), IID_PPV_ARGS(&m_CommandList)));
	// Command lists should only be opened on the main loop, so pre-emptively close it
	DX_CALL(m_CommandList->Close());

	// Vertex Buffer
	{
		Vertex triangleVertices[] =
		{
			{ { 0.f, 0.25f * CoreStatics::ViewportAspectRatio, 0.f }, { 1.f, 0.f, 0.f, 1.f } },
			{ { 0.25f, -0.25f * CoreStatics::ViewportAspectRatio, 0.f }, { 0.f, 1.f, 0.f, 1.f } },
			{ { -0.25f, -0.25 * CoreStatics::ViewportAspectRatio, 0.f }, { 0.f, 0.f, 1.f, 1.f } }
		};

		const u32 vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		const D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const D3D12_RESOURCE_DESC resourceBuffer = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		DX_CALL(m_Device->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceBuffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)));

		// Copy the triangle data to the vertex buffer
		u8* vertexDataBegin = nullptr;
		// No intention to read this resource on the CPU
		CD3DX12_RANGE readRange(0, 0);
		DX_CALL(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDataBegin)));
		memcpy(vertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_VertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(Vertex);
		m_VertexBufferView.SizeInBytes = vertexBufferSize;
	}
	
	// Synchronization Objects
	{
		DX_CALL(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_FenceValue = 1;

		m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (!m_FenceEvent)
		{
			DX_CALL(HRESULT_FROM_WIN32(GetLastError()));
		}

		// Wait for the command list to execute, even if we have a static command list
		WaitForPreviousFrame();
	}
}

void D3D12Renderer::GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter, bool bRequestHighPerformanceAdapter) const
{
	*adapter = nullptr;

	Core::ComPtr<IDXGIAdapter1> adapter1 = nullptr;
	Core::ComPtr<IDXGIFactory6> factory6 = nullptr;

	if (DX_CALL_SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory6))))
	{
		for (u32 index = 0; 
			DX_CALL_SUCCEEDED(factory6->EnumAdapterByGpuPreference(index,
				bRequestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&adapter1))); 
			++index)
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
			if (DX_CALL_SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	if (!adapter1.Get())
	{
		for (u32 index = 0; DX_CALL_SUCCEEDED(factory->EnumAdapters1(index, &adapter1)); ++index)
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
			if (DX_CALL_SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*adapter = adapter1.Detach();
}

void D3D12Renderer::PopulateCommandList()
{
	DX_CALL(m_CommandAllocator->Reset());
	DX_CALL(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineState.Get()));

	// Set necessary state
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Indicate the back buffer will be used as a render target
	const CD3DX12_RESOURCE_BARRIER& targetTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &targetTransition);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorHeapSize);
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// Record commands
	const float clearColor[] = { 0.f, 0.2f, 0.4f, 1.f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present
	const CD3DX12_RESOURCE_BARRIER& presentTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &presentTransition);

	DX_CALL(m_CommandList->Close());
}

void D3D12Renderer::WaitForPreviousFrame() 
{
	// todo: Switch to framebuffers or other ways to maximize GPU utilization

	// Signal and increment the fence value
	const u64 fenceValue = m_FenceValue;
	DX_CALL(m_CommandQueue->Signal(m_Fence.Get(), fenceValue));
	++m_FenceValue;

	// Wait until the previous frame is finished
	if (m_Fence->GetCompletedValue() < fenceValue)
	{
		DX_CALL(m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}
