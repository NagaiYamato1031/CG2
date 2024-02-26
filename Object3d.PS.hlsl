#include "object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t shininess;
};
struct DirectionalLight
{
    float32_t4 color; // ライトの色
    float32_t3 direction; // ライトの向き
    float intensity; // 輝度
};

struct Camera
{
    float32_t3 worldPosition;
};


Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

ConstantBuffer<Material> gMaterial : register(b0);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

ConstantBuffer<Camera> gCamera : register(b2);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
	//int32_t enableLighting : ENABLE_LIGHTING0;
};
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	/*// ランバート反射モデルを利用した Lighting の計算
	if (gMaterial.enableLighting != 0) {
		// Lighting する場合
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	}*/
    // 鏡面反射モデル
    if (gMaterial.enableLighting != 0)
    {
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        float RdotE = dot(reflectLight, toEye);
        float specularPow = pow(saturate(RdotE), gMaterial.shininess);
		// 拡散反射
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        // 鏡面反射
        float32_t3 specular = gDirectionalLight.color.rbg * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        // 拡散反射 + 鏡面反射
        output.color.rgb = diffuse + specular;
        // αは変わらない
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
		// Lighting しない場合。前回までと同じ演算
        output.color = gMaterial.color * textureColor;
    }
    return output;
}