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
	plane->SetTranslate({ 0.0f, 0.0f, 6.0f });

	// 3Dカメラの初期化
	camera1 = std::make_unique<Camera>();
	camera1->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera1->SetRotate({ 0.0f, 0.0f, 0.0f });

	// カメラのセット
	plane->SetCamera(camera1.get());
	particle->SetCamera(camera1.get());

	particle->Initialize();
	particle->CreateParticleGroup("particle", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{64.0f,64.0f});
	//particle->CreateParticleGroup("particle2", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{32.0f,32.0f});
	// ParticleEmitterの初期化
	auto emitter = std::make_unique<ParticleEmitter>(particle.get(), "particle", Transform{ {0.0f, 0.0f, -4.0f} }, 10, 0.5f, true);
	emitters.push_back(std::move(emitter));

	aabb.min = { -0.5f, -0.5f, 0.0f }; // AABB の最小点を少し下げる
	aabb.max = { 0.5f, 0.6f, 0.5f };  // AABB の最大点を少し上げる
	aabb.color = static_cast<int>(Color::WHITE); // AABBの色を赤に設定
	DrawLine::GetInstance()->SetCamera(camera1.get());
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
	camera1->Update();

	for (auto& emitter : emitters)
	{
		emitter->Update();
	}
	particle->Update();

	ImGui::Begin("Debug Information"); // デバッグ情報用ウィンドウ
	ImGui::Text("Number of Lines: %zu", DrawLine::GetInstance()->GetLineCount());
	ImGui::End();

	// ImGui を使用したカメラ制御
	ImGui::Begin("Camera Control");
	ImGui::Text("Use the sliders to control the camera.");
	Vector3 cameraPosition = camera1->GetTranslate();
	Vector3 cameraRotation = camera1->GetRotate();

	// カメラの位置を個別に操作
	if (ImGui::DragFloat("Position X", &cameraPosition.x, 0.1f)) {
		camera1->SetTranslate(cameraPosition);
	}
	if (ImGui::DragFloat("Position Y", &cameraPosition.y, 0.1f)) {
		camera1->SetTranslate(cameraPosition);
	}
	if (ImGui::DragFloat("Position Z", &cameraPosition.z, 0.1f)) {
		camera1->SetTranslate(cameraPosition);
	}

	// カメラの回転を個別に操作
	if (ImGui::DragFloat("Rotation X", &cameraRotation.x, 0.01f)) {
		camera1->SetRotate(cameraRotation);
	}
	if (ImGui::DragFloat("Rotation Y", &cameraRotation.y, 0.01f)) {
		camera1->SetRotate(cameraRotation);
	}
	if (ImGui::DragFloat("Rotation Z", &cameraRotation.z, 0.01f)) {
		camera1->SetRotate(cameraRotation);
	}
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

	// 各オブジェクトの描画
	plane->Draw();

	// ================================================
	// ここまで3Dオブジェクト個々の描画
	// ================================================

	// ================================================
	// ここからDrawLine個々の描画
	// ================================================

	/*DrawLine::GetInstance()->AddLine(
		{ 0.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.0f },
		Color::WHITE,
		Color::WHITE
	);*/

	DrawLine::GetInstance()->DrawAABB(aabb);

	// ================================================
	// ここまでDrawLine個々の描画
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
