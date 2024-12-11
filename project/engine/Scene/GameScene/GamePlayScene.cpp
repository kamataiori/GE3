#include "GamePlayScene.h"
#include <Input.h>
#include "SceneManager.h"
#include <SpriteCommon.h>

void GamePlayScene::Initialize()
{
	// ライト
	// Lightクラスのデータを初期化
	BaseScene::GetLight()->Initialize();
	BaseScene::GetLight()->GetCameraLight();
	BaseScene::GetLight()->GetDirectionalLight();
	BaseScene::GetLight()->SetDirectionalLightIntensity({ 1.0f });
	BaseScene::GetLight()->SetDirectionalLightColor({ 1.0f,1.0f,1.0f,1.0f });
	//BaseScene::GetLight()->SetDirectionalLightDirection(Normalize({ 1.0f,1.0f }));

	// 3Dカメラの初期化
	/*cameraTransform.translate = { 0.0f, 0.0f, 0.0f };
	camera1_->SetTranslate(cameraTransform.translate);
	cameraManager_->AddCamera(camera1_.get());*/

	skyDome_->Initialize();

	ground_->Initialize();
	
	// カメラのセット
	//skyDome_->->SetCameraManager(cameraManager_.get());
}

void GamePlayScene::Finalize()
{
	
}

void GamePlayScene::Update()
{
	//ImGui::Begin("camera");
	//ImGui::DragFloat3("translate", &cameraTransform.translate.x, 0.01f);
	//ImGui::DragFloat3("rotate", &cameraTransform.rotate.x, 0.01f);
	//ImGui::End();

	//// カメラの位置を更新
	//camera1_->SetTranslate(cameraTransform.translate);
	//camera1_->SetRotate(cameraTransform.rotate);


	skyDome_->Update();

	ground_->Update();

	// カメラの更新
	//camera1_->Update();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

}

void GamePlayScene::BackGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の背景描画
	// ================================================

	

	// ================================================
	// ここまでSprite個々の背景描画
	// ================================================
}

void GamePlayScene::Draw()
{
	// 3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
	Object3dCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここから3Dオブジェクト個々の描画
	// ================================================

	skyDome_->Draw();

	ground_->Draw();

	// ================================================
	// ここまで3Dオブジェクト個々の描画
	// ================================================
}

void GamePlayScene::ForeGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の前景描画(UIなど)
	// ================================================

	

	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}
