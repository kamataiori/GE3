#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"

void TitleScene::Initialize()
{
	// ==============================================
	//    BaseSceneがLightを持っているため
	//    LightのInitialize()は必ず必要
	// ==============================================

	// Lightクラスのデータを初期化
	BaseScene::GetLight()->Initialize();

	// 3Dオブジェクトの初期化
	plane = std::make_unique<Object3d>(this);
	plane->Initialize();
	//// モデル読み込み
	ModelManager::GetInstance()->LoadModel("uvChecker.gltf");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	plane->SetModel("uvChecker.gltf");

	// モデルにSRTを設定
	plane->SetScale({ 1.0f, 1.0f, 1.0f });
	plane->SetRotate({ 0.0f, 0.0f, 0.0f });
	plane->SetTranslate({ 2.0f, 0.0f, 0.0f });

	// 3Dカメラの初期化
	cameraManager = std::make_unique<CameraManager>();
	camera1->SetTranslate({ 0.0f, 0.0f, -15.0f });
	cameraManager->AddCamera(camera1.get());

	// カメラのセット
	plane->SetCameraManager(cameraManager.get());

}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	// 各3Dオブジェクトの更新
	plane->Update();
	// カメラの更新
	camera1->Update();


	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}

void TitleScene::Draw()
{
	// 3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
	Object3dCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここから3Dオブジェクト個々の描画
	// ================================================

	// 各オブジェクトの描画
	plane->Draw();

	// ================================================
	// ここまで3Dオブジェクト個々の描画
	// ================================================

	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の描画
	// ================================================


	// ================================================
	// ここまでSprite個々の描画
	// ================================================
}
