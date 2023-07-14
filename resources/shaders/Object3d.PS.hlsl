#include "Object3d.hlsli"

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	//output.color = gMaterial.color;
	output.color = input.color;
	return output;
}