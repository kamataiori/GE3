#pragma once
#include <vector>
#include "Camera.h"

class CameraManager {
public:
    /// <summary>
    /// カメラを追加
    /// </summary>
    void AddCamera(Camera* camera);

    /// <summary>
    /// カメラを取得
    /// </summary>
    Camera* GetCamera(int index);

    /// <summary>
    /// 現在のカメラをセット
    /// </summary>
    void SetCurrentCamera(int index);

    /// <summary>
    /// 現在のカメラを取得
    /// </summary>
    Camera* GetCurrentCamera() const;

    int GetCurrentCameraIndex();

private:
    std::vector<Camera*> cameras_;
    int currentCameraIndex_ = 0;
};
