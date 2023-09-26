struct Material {
	float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct TransformationMatrix {
	float32_t4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
	float32_t4 color : COLOR0;
};

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t4 color : COLOR0;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};