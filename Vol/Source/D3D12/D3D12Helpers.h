#pragma once

#define DX_CALL(HResult) { if (FAILED(HResult)) { verifyf(false, "DX_CALL failed, refer to DirectX12 trace for more information.") } } 