#pragma once

#include <d3dx12_core.h>
#include <wrl/client.h>

class D3D12Renderer
{
public:
	D3D12Renderer();
	~D3D12Renderer() = default;

	/// @brief Initializes the D3D12Renderer.
	void Init();

	/// @brief Perform a one-frame render pass.
	void Render();

	/// @brief Destroys the D3D12Renderer.
	void Destroy();

private:
	/// @brief Initializes the renderer's required DirectX pipelines.
	void InitPipelines();

	/// @brief Initializes the sample resources.
	/// @todo Split up to be more dynamic once sample is implemented
	void InitResources();
	
	/// @brief Looks up and retrieves the hardware adapter for the machine running DirectX12. 
	void GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter, bool bRequestHighPerformanceAdapter = false) const;

	/// @brief Populate the command list for rendering
	void PopulateCommandList();

	/// @brief Allows synchronization objects to wait 
	void WaitForPreviousFrame();

	struct Vertex
	{
		// todo: Create native math helpers
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

	u32 m_FrameIndex{ 0 };
	static constexpr u8 m_FrameCount{ 2 };

	Core::ComPtr<ID3D12Device> m_Device{ nullptr };
	Core::ComPtr<ID3D12CommandQueue> m_CommandQueue{ nullptr };
	Core::ComPtr<IDXGISwapChain3> m_SwapChain{ nullptr };

	Core::ComPtr<ID3D12Resource> m_RenderTargets[m_FrameCount];
	Core::ComPtr<ID3D12RootSignature> m_RootSignature{ nullptr };
	Core::ComPtr<ID3D12PipelineState> m_PipelineState{ nullptr };
	Core::ComPtr<ID3D12CommandAllocator> m_CommandAllocator{ nullptr };
	Core::ComPtr<ID3D12GraphicsCommandList> m_CommandList{ nullptr };

	Core::ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap{ nullptr };
	u32 m_RtvDescriptorHeapSize{ 0 };

	Core::ComPtr<ID3D12Resource> m_VertexBuffer{ nullptr };
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView{};

	Core::ComPtr<ID3D12Fence> m_Fence{ nullptr };
	HANDLE m_FenceEvent{ nullptr };
	u64 m_FenceValue{ 0 };

	CD3DX12_VIEWPORT m_Viewport;
	CD3DX12_RECT m_ScissorRect;
};