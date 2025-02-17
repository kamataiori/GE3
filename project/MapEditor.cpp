#include "MapEditor.h"
#include "ModelManager.h"
#include "BaseScene.h"

// マップエディタの初期化処理
void MapEditor::Initialize() {
    GlobalVariables::GetInstance()->LoadFiles(); // JSONデータのロード

    // 必要な3Dモデルのロード
    ModelManager::GetInstance()->LoadModel("axis.obj");
}

// カメラのセット
void MapEditor::SetCamera(Camera* camera) {
    camera_ = camera;
    for (auto& obj : objects_) {
        obj->SetCamera(camera_);
    }
}

// マップエディタの更新処理 (ImGui UIの管理を含む)
void MapEditor::Update() {
    for (auto& obj : objects_) {
        obj->Update(); // 各オブジェクトの更新処理
    }

    if (ImGui::Begin("Map Editor")) {
        if (ImGui::Button("Save")) {
            SaveMap("default_map"); // マップの保存
        }
        //if (ImGui::Button("Load")) {
        //    LoadMap("default_map"); // マップの読み込み
        //}
        ImGui::End();
    }
}

// マップ内のオブジェクトを描画
void MapEditor::Draw() {
    for (auto& obj : objects_) {
        obj->Draw(); // 各オブジェクトの描画処理
    }
}

// マップのデータをJSONに保存
void MapEditor::SaveMap(const std::string& fileName) {
    GlobalVariables* gv = GlobalVariables::GetInstance();
    gv->CreateGroup(fileName);

    for (size_t i = 0; i < objects_.size(); i++) {
        std::string objKey = "Object" + std::to_string(i);
        gv->SetValue(fileName, objKey + "_pos", objects_[i]->GetTranslate());
        gv->SetValue(fileName, objKey + "_rot", objects_[i]->GetRotate());
        gv->SetValue(fileName, objKey + "_scale", objects_[i]->GetScale());
    }

    gv->SaveFile(fileName); // JSONにデータを保存
}

// JSONデータを読み込み、マップを復元
void MapEditor::LoadMap(const std::string& fileName) {
    GlobalVariables* gv = GlobalVariables::GetInstance();
    gv->LoadFile(fileName);

    objects_.clear(); // 現在のオブジェクトをクリア

    for (size_t i = 0; i < 100; i++) { // 最大100個まで試す
        std::string objKey = "Object" + std::to_string(i);
        try {
            Vector3 pos = gv->GetValue<Vector3>(fileName, objKey + "_pos");
            Vector3 rot = gv->GetValue<Vector3>(fileName, objKey + "_rot");
            Vector3 scale = gv->GetValue<Vector3>(fileName, objKey + "_scale");

            auto newObj = std::make_unique<Object3d>(nullptr);
            newObj->SetTranslate(pos);
            newObj->SetRotate(rot);
            newObj->SetScale(scale);

            if (camera_) {
                newObj->SetCamera(camera_);
            }

            objects_.push_back(std::move(newObj));
        }
        catch (...) {
            break; // データがない場合は終了
        }
    }
}

// 新しいオブジェクトを追加する
void MapEditor::AddObject(BaseScene* scene, const std::string& modelPath, const Vector3& position) {
    // BaseScene を渡して Object3d を生成
    auto newObj = std::make_unique<Object3d>(scene);
    newObj->Initialize();
    newObj->SetModel(modelPath);
    newObj->SetTranslate(position);

    if (camera_) {
        newObj->SetCamera(camera_);
    }

    objects_.push_back(std::move(newObj));
}
