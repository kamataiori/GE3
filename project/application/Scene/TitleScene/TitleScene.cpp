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
	BaseScene::GetLight()->SetDirectionalLightDirection(Normalize({ 1.0f,1.0f }));

	BaseScene::GetLight()->SetCameraPosition({ 0.0f, 1.0f, 0.0f });

	BaseScene::GetLight()->GetSpotLight();
	BaseScene::GetLight()->SetSpotLightColor({ 1.0f,1.0f,1.0f,1.0f });
	BaseScene::GetLight()->SetSpotLightPosition({ 10.0f,2.25f,0.0f });
	BaseScene::GetLight()->SetSpotLightIntensity({ 4.0f });
	BaseScene::GetLight()->SetSpotLightCosAngle(std::cos(std::numbers::pi_v<float> / 3.0f));
	BaseScene::GetLight()->SetSpotLightDecay(2.0f);
	BaseScene::GetLight()->SetSpotLightDirection(Normalize({ -1.0f, -1.0f, 0.0f }));
	BaseScene::GetLight()->SetSpotLightDistance(7.0f);

	BaseScene::GetLight()->GetPointLight();
	BaseScene::GetLight()->SetPointLightColor({ 1.0f,1.0f,1.0f,1.0f });
	BaseScene::GetLight()->SetPointLightDecay(10.0f);
	BaseScene::GetLight()->SetPointLightIntensity(0.0f);
	BaseScene::GetLight()->SetPointLightPosition({ 0.0f, 2.0f, 0.0f });
	BaseScene::GetLight()->SetPointLightRadius(20.0f);

	// 3Dオブジェクトの初期化
	plane = std::make_unique<Object3d>(this);
	plane->Initialize();
	monster = std::make_unique<Object3d>(this);
	monster->Initialize();
	//// モデル読み込み
	ModelManager::GetInstance()->LoadModel("uvChecker.gltf");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	plane->SetModel("uvChecker.gltf");

	ModelManager::GetInstance()->LoadModel("Monster.obj");
	monster->SetModel("Monster.obj");

	// モデルにSRTを設定
	plane->SetScale({ 1.0f, 1.0f, 1.0f });
	plane->SetRotate({ 0.0f, 3.14f, 0.0f });
	plane->SetTranslate({ 6.0f, 0.0f, 6.0f });

	monster->SetScale({ 1.0f, 1.0f, 1.0f });
	monster->SetRotate({ 0.0f, 3.14f, 0.0f });
	monster->SetTranslate({ 0.0f, 0.0f, 6.0f });

	// 3Dカメラの初期化
	camera1 = std::make_unique<Camera>();
	camera1->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera1->SetRotate({ 0.0f, 0.0f, 0.0f });

	// カメラのセット
	plane->SetCamera(camera1.get());
	particle->SetCamera(camera1.get());
	monster->SetCamera(camera1.get());

	particle->Initialize();
	particle->CreateParticleGroup("particle", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{64.0f,64.0f});
	//particle->CreateParticleGroup("particle2", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{32.0f,32.0f});
	// ParticleEmitterの初期化
	auto emitter = std::make_unique<ParticleEmitter>(particle.get(), "particle", Transform{ {0.0f, 0.0f, -4.0f} }, 10, 0.5f, true);
	emitters.push_back(std::move(emitter));

	
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	BaseScene::ShowFPS();

	// 各3Dオブジェクトの更新
	plane->Update();
	monster->Update();
	camera1->Update();

	for (auto& emitter : emitters)
	{
		emitter->Update();
	}
	particle->Update();

	auto* light = BaseScene::GetLight();

	// DirectionalLight のUI
	ImGui::Begin("Directional Light");
	{
		ImGui::ColorEdit4("Color", &light->GetDirectionalLight()->color.x);
		ImGui::DragFloat3("Direction", &light->GetDirectionalLight()->direction.x, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat("Intensity", &light->GetDirectionalLight()->intensity, 0.1f, 0.0f, 10.0f);
	}
	ImGui::End();

	// SpotLight のUI
	ImGui::Begin("Spot Light");
	{
		ImGui::ColorEdit4("Color", &light->GetSpotLight()->color.x);
		ImGui::DragFloat3("Position", &light->GetSpotLight()->position.x, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat3("Direction", &light->GetSpotLight()->direction.x, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat("Intensity", &light->GetSpotLight()->intensity, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Cos Angle", &light->GetSpotLight()->cosAngle, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Decay", &light->GetSpotLight()->decay, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Distance", &light->GetSpotLight()->distance, 0.1f, 0.0f, 100.0f);
	}
	ImGui::End();

	// PointLight のUI
	ImGui::Begin("Point Light");
	{
		ImGui::ColorEdit4("Color", &light->GetPointLight()->color.x);
		ImGui::DragFloat3("Position", &light->GetPointLight()->position.x, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat("Intensity", &light->GetPointLight()->intensity, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Radius", &light->GetPointLight()->radius, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Decay", &light->GetPointLight()->decay, 0.1f, 0.0f, 10.0f);
	}
	ImGui::End();

	// Camera Position のUI
	ImGui::Begin("Camera Position");
	{
		ImGui::DragFloat3("Position", &light->GetCameraLight()->worldPosition.x, 0.1f, -100.0f, 100.0f);
	}
	ImGui::End();

	// Debug情報表示
	ImGui::Begin("Debug Information");
	ImGui::Text("Number of Lines: %zu", DrawLine::GetInstance()->GetLineCount());
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
	monster->Draw();

	// ================================================
	// ここまで3Dオブジェクト個々の描画
	// ================================================

	// ================================================
	// ここからDrawLine個々の描画
	// ================================================

	DrawLine::GetInstance()->AddLine(
		{ -0.5f, 0.0f, 0.0f },
		{ -1.5f, 0.5f, 0.0f },
		Color::WHITE,
		Color::WHITE
	);

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

	//particle->Draw();

	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}
