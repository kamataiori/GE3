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
	//BaseScene::GetLight()->SetDirectionalLightDirection(Normalize({ 1.0f,1.0f }));
	/*BaseScene::GetLight()->GetSpotLight();
	BaseScene::GetLight()->SetCameraPosition({ 0.0f, 1.0f, 0.0f });
	BaseScene::GetLight()->SetSpotLightColor({ 1.0f,1.0f,1.0f,1.0f });
	BaseScene::GetLight()->SetSpotLightPosition({ 10.0f,2.25f,0.0f });
	BaseScene::GetLight()->SetSpotLightIntensity({ 4.0f });*/

	// CameraManagerを初期化
	cameraManager_ = std::make_unique<CameraManager>();
	// カメラ1: メインカメラ
	auto mainCamera = new Camera();
	mainCamera->SetTranslate({ 0.0f, 0.0f, -20.0f });
	mainCamera->Update();
	cameraManager_->AddCamera(mainCamera);

	// カメラ2: 上からの視点
	auto topCamera = new Camera();
	topCamera->SetTranslate({ 0.0f, 0.0f, -10.0f });
	topCamera->Update();
	cameraManager_->AddCamera(topCamera);

	// カメラ3: 斜め視点
	auto diagonalCamera = new Camera();
	diagonalCamera->SetTranslate({ 0.0f, 0.0f, -5.0f });
	diagonalCamera->Update();
	cameraManager_->AddCamera(diagonalCamera);

	// 最初のカメラを設定
	cameraManager_->SetCurrentCamera(0);

	// Object3dCommon に CameraManager を設定
	Object3dCommon::GetInstance()->SetCameraManager(cameraManager_.get());

	// 3Dオブジェクトの初期化
	plane = std::make_unique<Object3d>(this);
	plane->Initialize();
	//// モデル読み込み
	ModelManager::GetInstance()->LoadModel("uvChecker.gltf");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	plane->SetModel("uvChecker.gltf");

	// モデルにSRTを設定
	plane->SetScale({ 1.0f, 1.0f, 1.0f });
	plane->SetRotate({ 0.0f, 3.14f, 0.0f });
	plane->SetTranslate({ -2.0f, 0.0f, 0.0f });

	//particle->Initialize();
	//particle->CreateParticleGroup("particle", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{64.0f,64.0f});
	////particle->CreateParticleGroup("particle2", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{32.0f,32.0f});
	//// ParticleEmitterの初期化
	//auto emitter = std::make_unique<ParticleEmitter>(particle.get(), "particle", Transform{ {0.0f, 0.0f, -4.0f} }, 10, 0.5f, true);
	//emitters.push_back(std::move(emitter));

}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	BaseScene::ShowFPS();

	//// アルファ値を減少させる
	//Vector4 color = plane->GetMaterialColor();
	////color.w = 0.5f;
	//color.w -= 0.01f; // アルファ値を減少
	//if (color.w < 0.0f) {
	//	color.w = 0.0f; // 最小値を0に制限
	//}
	//plane->SetMaterialColor(color);

	// 各3Dオブジェクトの更新
	plane->Update();
	// カメラの更新
	//camera->Update();

	/*for (auto& emitter : emitters)
	{
		emitter->Update();
	}
	particle->Update();*/

	// カメラの名前を画面に表示
	ImGui::Begin("Camera Info");
	ImGui::Text("Current Camera: %d", cameraManager_->GetCurrentCameraIndex());
	ImGui::End();

	// 入力でカメラを切り替え
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		cameraManager_->SetCurrentCamera(0);  // メインカメラ
	}
	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		cameraManager_->SetCurrentCamera(1);  // 上からの視点
	}
	if (Input::GetInstance()->TriggerKey(DIK_3)) {
		cameraManager_->SetCurrentCamera(2);  // 斜め視点
	}

	// カメラの移動例
	if (cameraManager_->GetCurrentCamera()) {
		Camera* currentCamera = cameraManager_->GetCurrentCamera();
		Vector3 position = currentCamera->GetTranslate();
		position.z += 0.1f;  // 毎フレーム前進
		currentCamera->SetTranslate(position);
	}

	// 全カメラの更新
	cameraManager_->UpdateAllCameras();

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

	// 各オブジェクトの描画
	plane->Draw();

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

	/*particle->Draw();*/

	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}
