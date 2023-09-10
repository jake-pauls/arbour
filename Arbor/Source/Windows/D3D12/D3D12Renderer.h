#pragma once

#ifdef WIN32
#include <wrl/client.h>
#endif

class D3D12Renderer
{
public:
	D3D12Renderer() = default;
	~D3D12Renderer() = default;

	/**
	 * @brief Initializes the D3D12Renderer.
	 */
	void Init();

	/**
	 * @brief Perform a one-frame render pass.
	 */
	void Render() const;

	/**
	 * @brief Destroys the D3D12Renderer.
	 */
	void Destroy() const;

private:
	/**
	 * @brief Initializes the renderer's required DirectX pipelines.
	 */
	void InitPipelines();

	/**
	 * @brief Initializes the sample assets.
	 * @todo Split up to be more dynamic once sample is implemented
	 */
	void InitAssets();
	
	/**
	 * @brief Looks up and retrieves the hardware adapter for the machine running DirectX12. 
	 */
	void GetHardwareAdapter(IDXGIFactory1* Factory, IDXGIAdapter1** Adapter, bool bRequestHighPerformanceAdapter = false) const;

	uint32_t FrameIndex{ 0 };
	static constexpr uint8_t FrameCount{ 2 };

	Core::ComPtr<ID3D12Device> Device{ nullptr };
	Core::ComPtr<ID3D12CommandQueue> CommandQueue{ nullptr };
	Core::ComPtr<IDXGISwapChain3> SwapChain{ nullptr };

	Core::ComPtr<ID3D12Resource> RenderTargets[FrameCount];
	Core::ComPtr<ID3D12CommandAllocator> CommandAllocator;

	Core::ComPtr<ID3D12DescriptorHeap> RtvDescriptorHeap{ nullptr };
	uint32_t RtvDescriptorHeapSize{ 0 };
};