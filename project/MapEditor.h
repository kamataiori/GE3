#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Object3d.h"
#include "GlobalVariables.h"
#include "Camera.h"

class BaseScene;

class MapEditor {
public:
    // 初期化処理 (JSONデータの読み込み)
    void Initialize();

    // 更新処理 (オブジェクトの更新 & ImGui UIの管理)
    void Update();

    // 描画処理 (全てのオブジェクトを描画)
    void Draw();

    // マップデータをJSONに保存
    void SaveMap(const std::string& fileName);

    // JSONデータを読み込んでマップを復元
    void LoadMap(const std::string& fileName);

    // 新しいオブジェクトを追加
    void AddObject(BaseScene* scene, const std::string& modelPath, const Vector3& position);

    // カメラのセット
    void SetCamera(Camera* camera);

private:
    // マップ内の3Dオブジェクトを保持するコンテナ
    std::vector<std::unique_ptr<Object3d>> objects_;
    Camera* camera_ = nullptr;
};
