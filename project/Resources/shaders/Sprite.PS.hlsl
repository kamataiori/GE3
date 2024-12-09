#include "Sprite.hlsli"

//ConstantBufferを使って色を指定する
struct Material
{
    float4 color : SV_TARGET0;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
   // output.color = float32_t4(1.0, 1.0, 1.0, 1.0);
    //output.color = gMaterial.color;
    output.color = gMaterial.color * textureColor;
    return output;
}

//float4 main() : SV_TARGET
//{
//    return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

//ConstantBufferを使って色を指定する
//struct Material
//{
//    float4 color;
//};

//ConstantBuffer<Material> gMaterial : register(b0);
//struct PixelShaderOutput
//{
//    float4 color : SV_TARGET0;
//};

//PixelShaderOutput main()
//{
//    PixelShaderOutput output;
//   // output.color = float32_t4(1.0, 1.0, 1.0, 1.0);
//    output.color = gMaterial.color;
//    return output;
//}