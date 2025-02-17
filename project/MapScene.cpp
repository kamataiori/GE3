#include "MapScene.h"

void MapScene::Initialize()
{
    // ==============================================
    //    BaseSceneがLightを持っているため
    //    LightのInitialize()は必ず必要
    // ==============================================

    // Lightクラスのデータを初期化
    BaseScene::GetLight()->Initialize();
    BaseScene::GetLight()->GetCameraLight();
    BaseScene::GetLight()->GetDirectionalLight();
    BaseScene::GetLight()->SetDirectionalLightIntensity({ 1.0f });
    BaseScene::GetLight()->SetDirectionalLightColor({ 1.0f,1.0f,1.0f,1.0f });

    // カメラを初期化
    camera_ = std::make_unique<Camera>();
    camera_->SetTranslate({ 0.0f, 0.0f, -20.0f });
    camera_->SetRotate({ 0.0f, 0.0f, 0.0f });

    // 初期化
    mapEditor.Initialize();

    // MapEditor にカメラを設定
    mapEditor.SetCamera(camera_.get());

    // オブジェクトの追加
    mapEditor.AddObject(this,"axis.obj", { 0.0f, 0.0f, -3.0f });
    //mapEditor.AddObject("Resources/Models/rock.obj", { 0.0f, 0.0f, 0.0f });


    GlobalVariables::GetInstance()->AddValue<Vector3>("MAPTEST", "position", camera_->GetTranslate());
}

void MapScene::Finalize()
{
}

void MapScene::Update()
{
    camera_->SetTranslate(GlobalVariables::GetInstance()->GetValue<Vector3>("MAPTEST", "position"));
    camera_->Update(); // カメラの更新
    mapEditor.Update(); // マップエディタの更新
}

void MapScene::BackGroundDraw()
{
}

void MapScene::Draw()
{
    // 3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->CommonSetting();

    mapEditor.Draw(); // マップエディタの描画
}

void MapScene::ForeGroundDraw()
{
}
