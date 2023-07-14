struct Material {
	float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
};

struct VertexShaderInput {
	float32_t4 position : POSITION0;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};