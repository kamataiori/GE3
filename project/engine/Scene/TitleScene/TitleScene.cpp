#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "ImGuiManager.h"

void TitleScene::Initialize()
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


	// 3Dカメラの初期化
	cameraManager = std::make_unique<CameraManager>();
	camera1->SetTranslate({ 0.0f, 0.0f, -15.0f });
	cameraManager->AddCamera(camera1.get());

	// カメラのセット
	particle->SetCameraManager(cameraManager.get());

	// パーティクルの初期化
	particle->Initialize();
	particle->CreateParticleGroup("water", "Resources/water.png", ParticleManager::BlendMode::kBlendModeAdd, { 32.0f, 32.0f });

	// ParticleEmitterの初期化（反時計回り）
	auto emitter = std::make_unique<ParticleEmitter>(
		particle.get(),
		"water",
		Transform{ {0.0f, 0.0f, -5.0f}, {}, {1.0f, 0.1f, 1.0f} },
		20, // パーティクル数
		0.1f, // 発生頻度
		true // 繰り返し
	);
	emitters.push_back(std::move(emitter));

}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	//BaseScene::ShowFPS();

	// カメラの更新
	camera1->Update();

	// パーティクルの更新
	for (auto& emitter : emitters)
	{
		// 反時計回りの回転を加える
		emitter->transform_.rotate.z += 0.05f;
		if (emitter->transform_.rotate.z > 360.0f) {
			emitter->transform_.rotate.z -= 360.0f;
		}
		emitter->Update();
	}
	particle->Update();

	ImGui::Begin("Editor");
	ImGui::Text("Enter to go to Editor");
	ImGui::End();

	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}

void TitleScene::BackGroundDraw()
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

void TitleScene::Draw()
{
	// 3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
	Object3dCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここから3Dオブジェクト個々の描画
	// ================================================

	

	// ================================================
	// ここまで3Dオブジェクト個々の描画
	// ================================================
}

void TitleScene::ForeGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の前景描画(UIなど)
	// ================================================

	particle->Draw();

	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}
