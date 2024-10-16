#include "Object3d.hlsli"

// ConstantBufferを使って色を指定する
struct Material
{
    float4 color : SV_TARGET0;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};

// 平行光源
struct DirectionalLight
{
    float4 color; //!<ライトの色
    float3 direction; //!<ライトの向き
    float intensity; //!<輝度
};

// ポイントライト
struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};

// TransformationMatrixを拡張する
struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Camera
{
    float3 worldPosition;
};

ConstantBuffer<Camera> gCamera : register(b2);

ConstantBuffer<PointLight> gPointLight : register(b3);

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    // textureのα値が0.5以下のときにPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }

    PixelShaderOutput output;

    //////=========Lightingの計算を行う=========////

    if (gMaterial.enableLighting != 0)
    {
        // カメラへの方向ベクトルを計算
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

        // 物体表面の特定の点に対する入射光を計算する
        float3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);

        // 距離を計算して減衰係数を取得する
        float3 lightToPixel = gPointLight.position - input.worldPosition;
        float distance = length(lightToPixel);
        float factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay);

        // PointLightの色を利用する（減衰を反映）
        float3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * saturate(dot(normalize(input.normal), -pointLightDirection)) * gPointLight.intensity * factor;
        
        // 平行光源からの拡散反射と鏡面反射
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;

        // 反射ベクトルの計算
        float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        float RdotE = dot(reflectLight, toEye);
        float specularPow = pow(saturate(RdotE), gMaterial.shininess);
        float3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;

        reflectLight = reflect(pointLightDirection, normalize(input.normal));
        RdotE = dot(reflectLight, toEye);
        specularPow = pow(saturate(RdotE), gMaterial.shininess);
        float3 specularPointLight = gPointLight.color.rgb * gPointLight.intensity * specularPow * factor;
        // 全部足して最終的な色を計算する
        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        // Lightしない場合
        output.color = gMaterial.color * textureColor;
    }

    return output;
}
