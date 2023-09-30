#pragma once

#define DX_CALL(hResult) \
	{ \
		if (FAILED(hResult)) \
		{ \
			verifyf(false, "DX_CALL failed, refer to DirectX12 trace for more information."); \
		} \
	} \

#define DX_CALL_SUCCEEDED(hResult) SUCCEEDED(hResult)

#define DX_CALL_FAILED(hResult) FAILED(hResult)
