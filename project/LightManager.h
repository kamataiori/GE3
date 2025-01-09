#pragma once
#include "Light.h"
#include <memory>

class LightManager
{
public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    static LightManager* GetInstance();

    /// <summary>
    /// ライトの初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// ライトのゲッター
    /// </summary>
    Light* GetLight() const { return light_.get(); }

    LightManager();
    ~LightManager() = default;

    // コピー禁止
    LightManager(const LightManager&) = delete;
    LightManager& operator=(const LightManager&) = delete;

    /// <summary>
    /// 終了
    /// </summary>
    void Finalize();

private:
    static std::unique_ptr<LightManager> instance_; // シングルトンインスタンス
    std::unique_ptr<Light> light_;                 // ライトインスタンス
};
