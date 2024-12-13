#pragma once
#include <vector>
#include "Camera.h"

class CameraManager {
public:
    // カメラを登録
    void AddCamera(Camera* camera) {
        cameras_.emplace_back(camera);
    }

    // 現在のカメラを設定
    void SetCurrentCamera(size_t index) {
        if (index < cameras_.size()) {
            currentCamera_ = cameras_[index];
        }
    }

    // 現在のカメラを取得
    Camera* GetCurrentCamera() const {
        return currentCamera_;
    }

    // 全カメラを更新
    void UpdateAllCameras() {
        for (auto& camera : cameras_) {
            camera->Update();
        }
    }

    size_t GetCurrentCameraIndex() const {
        auto it = std::find(cameras_.begin(), cameras_.end(), currentCamera_);
        return (it != cameras_.end()) ? std::distance(cameras_.begin(), it) : -1;
    }


private:
    std::vector<Camera*> cameras_;  // カメラリスト
    Camera* currentCamera_ = nullptr;  // 現在のカメラ
};
