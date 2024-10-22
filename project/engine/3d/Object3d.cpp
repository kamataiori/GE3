#include "Object3d.h"
#include "MathFunctions.h"
#include "TextureManager.h"
#define _USE_MATH_DEFINES
#include "GamePlayScene.h"

Object3d::Object3d(GamePlayScene* scene)
{
    gamePlayScene_ = scene;
}

Object3d::Object3d(TitleScene* scene)
{
    titleScene_ = scene;
}

void Object3d::Initialize()
{
    // 引数で受け取ってメンバ変数に記録する
    this->object3dCommon_ = Object3dCommon::GetInstance();

    // 座標変換行列データの初期化
    CreateTransformationMatrixData();

    // Transform変数を作る
    transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
    cameraTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 3.14f, 0.0f}, {0.0f, 1.0f, -10.0f} };

    // デフォルトカメラをセット
    this->camera = object3dCommon_->GetDefaultCamera();
}

void Object3d::Update()
{
    // model_ から modelData を取得
    const Model::ModelData& modelData = model_->GetModelData();

    // TransformからworldMatrixを作る
    Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

    // カメラTransformからカメラ行列を作る
    if (cameraManager_) {
        Camera* camera = cameraManager_->GetCurrentCamera();
        if (camera) {
            Matrix4x4 viewProjectionMatrix = camera->GetViewProjectionMatrix();
            worldviewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
        }
    }
    else {
        worldviewProjectionMatrix = worldMatrix;
    }

    transformationMatrixData->WVP = Multiply(modelData.rootNode.localMatrix, worldviewProjectionMatrix);
    transformationMatrixData->World = Multiply(modelData.rootNode.localMatrix, worldMatrix);
}

void Object3d::ImGuiUpdate(const std::string& Name)
{
    ImGui::Begin("object3d");
    if (ImGui::TreeNode(("object3d_" + Name).c_str())) {
        ImGui::DragFloat3("translate", &transform.translate.x);
        ImGui::DragFloat3("scale", &transform.scale.x);
        ImGui::DragFloat3("rotate", &transform.rotate.x);
        ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 0.01f);
        ImGui::DragFloat3("CameraTransform", &cameraTransform.translate.x, 0.01f);

        //// DirectionalLight の更新
        //ImGui::DragFloat3("DirectionalLight Direction", &lightData->GetDirectionalLight().direction.x, 0.01f);
        //ImGui::DragFloat("DirectionalLight Intensity", &lightData->GetDirectionalLight().intensity, 0.01f);

        //// SpotLight の更新
        //ImGui::DragFloat3("SpotLight Position", &lightData->GetSpotLight().position.x, 0.01f);
        //ImGui::DragFloat3("SpotLight Direction", &lightData->GetSpotLight().direction.x, 0.01f);
        //ImGui::DragFloat("SpotLight Intensity", &lightData->GetSpotLight().intensity, 0.01f);

        ImGui::TreePop();
    }
    ImGui::End();
}

void Object3d::Draw()
{
    // 座標変換行列CBufferの場所を設定
    object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

    object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, gamePlayScene_->GetLight()->GetDirectionalLightGPUAddress());

    object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, gamePlayScene_->GetLight()->GetCameraLightGPUAddress());

    object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, gamePlayScene_->GetLight()->GetPointLightGPUAddress());

    object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, gamePlayScene_->GetLight()->GetSpotLightGPUAddress());

    // 3Dモデルが割り当てられていたら描画する
    if (model_) {
        model_->Draw();
    }
}

void Object3d::CreateTransformationMatrixData()
{
    // 座標変換行列用のリソースを作成
    transformationMatrixResource = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

    // 書き込むためのアドレスを取得
    transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

    // 単位行列を書き込んでおく
    transformationMatrixData->WVP = MakeIdentity4x4();
    transformationMatrixData->World = MakeIdentity4x4();
    transformationMatrixData->WorldInverseTranspose = MakeIdentity4x4();
}

void Object3d::SetCameraManager(CameraManager* cameraManager)
{
    this->cameraManager_ = cameraManager;
}