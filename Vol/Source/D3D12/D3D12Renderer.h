#pragma once

class D3D12Renderer
{
public:
	D3D12Renderer() = default;
	~D3D12Renderer() = default;

	/**
	 * @brief Initializes the D3D12Renderer.
	 */
	void Init() const;

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
	void InitPipelines() const;
	
	/**
	 * @brief Looks up and retrieves the hardware adapter for the machine running DirectX12. 
	 */
	void GetHardwareAdapter(IDXGIFactory1* Factory, IDXGIAdapter1** Adapter, bool bRequestHighPerformanceAdapter = false) const;

	ID3D12Device* Device{ nullptr };
};