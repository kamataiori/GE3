#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <numbers>

class Light {
public:
    struct DirectionalLight {
        Vector4 color;  //!< ライトの色
        Vector3 direction;  //!< ライトの向き
        float intensity;  //!< 輝度
    };

    struct CameraForGPU {
        Vector3 worldPosition;
    };

    struct PointLight {
        Vector4 color;   //!< ライトの色
        Vector3 position;  //!< ライトの位置
        float intensity;  //!< 輝度
        float radius;  //! ライトの届く最大距離
        float decay;  //! 減衰率
    };

    struct SpotLight {
        Vector4 color;  //!< ライトの色
        Vector3 position;  //!< ライトの位置
        float intensity;  //!< 輝度
        Vector3 direction;  //!< スポットライトの方向
        float distance;  //!< ライトの届く最大距離
        float decay;  //! 減衰率
        float cosAngle;  //!< スポットライトの余弦
        float padding[2];  //!< パディング
    };

    DirectionalLight directionalLightData;
    CameraForGPU cameraLightData;
    PointLight pointLightData;
    SpotLight spotLightData;

    Light();
    ~Light() = default;

    void InitializeDirectionalLight();
    void InitializeCameraLight();
    void InitializePointLight();
    void InitializeSpotLight();

    // 各ライトのセット・ゲット関数
    DirectionalLight& GetDirectionalLight();
    CameraForGPU& GetCameraLight();
    PointLight& GetPointLight();
    SpotLight& GetSpotLight();
};
