#include "D3D12Renderer.h"

#include "D3D12Helpers.h"

void D3D12Renderer::Init() const
{
	InitPipelines();
}

void D3D12Renderer::Render() const
{
}

void D3D12Renderer::Destroy() const
{
}

void D3D12Renderer::InitPipelines() const
{
	uint32_t DxgiFactoryFlags = 0;

#if VOL_DEBUG
	// Enable debug mode for D3D12
	{
		ID3D12Debug* DebugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
		{
			DebugController->EnableDebugLayer();
			DxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	IDXGIFactory4* Factory;
	DX_CALL(CreateDXGIFactory2(DxgiFactoryFlags, IID_PPV_ARGS(&Factory)));

	// todo: add warp adapter? 
	// https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp

	IDXGIAdapter1* HardwareAdapter;
	GetHardwareAdapter(Factory, &HardwareAdapter);
	// todo: remove this const_cast
	DX_CALL(D3D12CreateDevice(HardwareAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(const_cast<ID3D12Device**>(&Device))));
	is(Device);
}

void D3D12Renderer::GetHardwareAdapter(IDXGIFactory1* Factory, IDXGIAdapter1** Adapter, bool bRequestHighPerformanceAdapter) const
{
	*Adapter = nullptr;

	IDXGIAdapter1* Adapter1 = nullptr;
	IDXGIFactory6* Factory6 = nullptr;
	
	if (SUCCEEDED(Factory->QueryInterface(IID_PPV_ARGS(&Factory6))))
	{
		for (uint32_t Index = 0; 
			SUCCEEDED(Factory6->EnumAdapterByGpuPreference(Index,
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
			if (SUCCEEDED(D3D12CreateDevice(Adapter1, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	if (!Adapter1)
	{
		for (uint32_t Index = 0; SUCCEEDED(Factory->EnumAdapters1(Index, &Adapter1)); ++Index)
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
			if (SUCCEEDED(D3D12CreateDevice(Adapter1, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*Adapter = Adapter1;
}
