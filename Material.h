#pragma once

#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <wrl.h>



struct MaterialStruct
{
	int32_t enableLighting = true;
};

class Material
{
public:
	MaterialStruct* data_;

	Material();
	~Material() = default;

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

};

