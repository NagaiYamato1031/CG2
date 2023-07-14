#include "Object3d.hlsli"

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	//output.position = mul(input.position, gTransformationMatrix.WVP);
	output.position = input.position;
	output.color = input.color;
	return output;
}