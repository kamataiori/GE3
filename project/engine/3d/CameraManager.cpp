#include "CameraManager.h"

void CameraManager::AddCamera(Camera* camera) {
    cameras_.push_back(camera);
}

Camera* CameraManager::GetCamera(int index) {
    if (index >= 0 && index < cameras_.size()) {
        return cameras_[index];
    }
    return nullptr;
}

void CameraManager::SetCurrentCamera(int index) {
    if (index >= 0 && index < cameras_.size()) {
        currentCameraIndex_ = index;
    }
}

Camera* CameraManager::GetCurrentCamera() const {
    if (!cameras_.empty()) {
        return cameras_[currentCameraIndex_];
    }
    return nullptr;
}

int CameraManager::GetCurrentCameraIndex()
{
    return currentCameraIndex_;
}
