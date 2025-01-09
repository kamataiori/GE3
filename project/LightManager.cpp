#include "LightManager.h"

// シングルトンインスタンスの初期化
std::unique_ptr<LightManager> LightManager::instance_ = nullptr;

LightManager* LightManager::GetInstance()
{
    if (!instance_) {
        instance_ = std::make_unique<LightManager>();
    }
    return instance_.get();
}

LightManager::LightManager()
{
    light_ = std::make_unique<Light>();
}

void LightManager::Finalize()
{
    light_.reset();
}

void LightManager::Initialize()
{
    light_->Initialize();
}
