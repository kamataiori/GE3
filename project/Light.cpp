#include "Light.h"
#include <numbers>
#include <MathFunctions.h>

Light::Light() {
    InitializeDirectionalLight();
    InitializeCameraLight();
    InitializePointLight();
    InitializeSpotLight();
}

void Light::InitializeDirectionalLight() {
    directionalLightData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData.direction = { 0.0f, -1.0f, 0.0f };
    directionalLightData.intensity = 1.0f;
}

void Light::InitializeCameraLight() {
    cameraLightData.worldPosition = { 0.0f, 0.0f, 0.0f };
}

void Light::InitializePointLight() {
    pointLightData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    pointLightData.position = { 0.0f, 0.0f, 0.0f };
    pointLightData.intensity = 1.0f;
    pointLightData.radius = 20.0f;
    pointLightData.decay = 10.0f;
}

void Light::InitializeSpotLight() {
    spotLightData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    spotLightData.position = { 0.0f, 0.0f, 0.0f };
    spotLightData.distance = 7.0f;
    spotLightData.direction = Normalize({ -1.0f, -1.0f, 0.0f });
    spotLightData.intensity = 4.0f;
    spotLightData.decay = 1.0f;
    spotLightData.cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
}

// 各ライトのゲッタ
Light::DirectionalLight& Light::GetDirectionalLight() {
    return directionalLightData;
}

Light::CameraForGPU& Light::GetCameraLight() {
    return cameraLightData;
}

Light::PointLight& Light::GetPointLight() {
    return pointLightData;
}

Light::SpotLight& Light::GetSpotLight() {
    return spotLightData;
}
