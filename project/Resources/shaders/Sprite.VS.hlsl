#include "Sprite.hlsli"

//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos;
//}

struct TransformationMatrix
{
    float4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

//struct VertexShaderOutput
//{
//	float32_t4 position : SV_POSITION;
//};

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    //output.position = input.position;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    return output;
}

//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//    return pos;
//}

//struct TransformationMatrix
//{
//    float4 WVP;
//};
//ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

//struct VertexShaderOutput
//{
//    float4 position : SV_POSITION;
//};

//struct VertexShaderInput
//{
//    float4 position : POSITION0;
//};

//VertexShaderOutput main(VertexShaderInput input)
//{
//    VertexShaderOutput output;
//    //output.position = input.position;
//    output.position = mul(input.position, gTransformationMatrix.WVP);
//    return output;
//}