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
	void GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter, bool bRequestHighPerformanceAdapter = false) const;

	uint32_t m_FrameIndex{ 0 };
	static constexpr uint8_t m_FrameCount{ 2 };

	Core::ComPtr<ID3D12Device> m_Device{ nullptr };
	Core::ComPtr<ID3D12CommandQueue> m_CommandQueue{ nullptr };
	Core::ComPtr<IDXGISwapChain3> m_SwapChain{ nullptr };

	Core::ComPtr<ID3D12Resource> m_RenderTargets[m_FrameCount];
	Core::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;

	Core::ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap{ nullptr };
	uint32_t m_RtvDescriptorHeapSize{ 0 };
};