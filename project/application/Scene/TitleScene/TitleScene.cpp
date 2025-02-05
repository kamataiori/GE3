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

	DrawLine::GetInstance()->SetCamera(camera1.get());
	aabb.min = { -1.8f, 2.2f, 3.0f }; // AABB の最小点を少し下げる
	aabb.max = { 1.8f, 1.1f, 0.5f };  // AABB の最大点を少し上げる
	aabb.color = static_cast<int>(Color::WHITE); // AABBの色を赤に設定
	sphere = { {-4.0f, -1.2f, 0.0f}, 1.0f, static_cast<int>(Color::WHITE) };
	ground.normal = { 0.0f, 1.0f, 0.0f }; // Y軸方向を法線とする平面
	ground.distance = -2.0f;             // 原点を通る平面
	ground.size = 6.0f;        // 平面のサイズ
	ground.divisions = 10;     // グリッドの分割数
	// カプセルの初期値
	capsule.start = { 1.6f, 0.0f, 0.0f };
	capsule.end = { 1.6f, -1.5f, 0.0f };
	capsule.radius = 0.5f;
	capsule.color = static_cast<int>(Color::WHITE);
	capsule.segments = 16; // 円周を構成する分割数
	capsule.rings = 8;     // 球部分を構成する分割数
	// OBB の初期化
	obb.center = { -1.9f, -0.3f, 0.0f };
	obb.orientations[0] = { 1.0f, 0.0f, 0.0f }; // X軸
	obb.orientations[1] = { 0.0f, 1.0f, 0.0f }; // Y軸
	obb.orientations[2] = { 0.0f, 0.0f, 1.0f }; // Z軸
	obb.size = { 1.0f, 1.0f, 0.5f }; // 各軸方向の半サイズ
	obb.color = static_cast<int>(Color::WHITE); // 色の初期値
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

	// AABB の編集
	ImGui::Begin("AABB Control");
	ImGui::Text("Adjust AABB parameters:");
	ImGui::DragFloat3("Min", &aabb.min.x, 0.1f); // AABB の最小点を調整
	ImGui::DragFloat3("Max", &aabb.max.x, 0.1f); // AABB の最大点を調整
	ImGui::End();

	// OBB の調整
	ImGui::Begin("OBB Control");
	ImGui::DragFloat3("Center", &obb.center.x, 0.1f); // 中心点
	ImGui::DragFloat3("Size", &obb.size.x, 0.1f, 0.1f, 10.0f); // 各軸方向の半サイズ
	ImGui::DragFloat3("Orientation X", &obb.orientations[0].x, 0.1f); // X軸方向
	ImGui::DragFloat3("Orientation Y", &obb.orientations[1].x, 0.1f); // Y軸方向
	ImGui::DragFloat3("Orientation Z", &obb.orientations[2].x, 0.1f); // Z軸方向
	ImGui::End();

	// Sphere の編集
	ImGui::Begin("Sphere Control");
	ImGui::Text("Adjust Sphere parameters:");
	ImGui::DragFloat3("Center", &sphere.center.x, 0.1f); // Sphere の中心点を調整
	ImGui::DragFloat("Radius", &sphere.radius, 0.1f, 0.1f, 100.0f); // Sphere の半径を調整
	ImGui::End();

	// Plane の調整
	ImGui::Begin("Ground Control");
	ImGui::Text("Adjust Plane parameters:");
	ImGui::DragFloat3("Normal", &ground.normal.x, 0.1f); // 法線を調整
	ImGui::DragFloat("Distance", &ground.distance, 0.1f); // 距離を調整
	ImGui::DragFloat("Size", &ground.size, 0.1f, 1.0f, 20.0f); // サイズを調整
	ImGui::DragInt("Divisions", &ground.divisions, 1, 1, 50); // グリッド分割数を調整
	ImGui::End();

	// Capsule の編集
	ImGui::Begin("Capsule Control");
	ImGui::DragFloat3("Start", &capsule.start.x, 0.1f); // 開始点を調整
	ImGui::DragFloat3("End", &capsule.end.x, 0.1f);     // 終了点を調整
	ImGui::DragFloat("Radius", &capsule.radius, 0.1f, 0.1f, 10.0f); // 半径を調整
	ImGui::DragInt("Segments", &capsule.segments, 1, 4, 64); // 円周分割数を調整
	ImGui::DragInt("Rings", &capsule.rings, 1, 2, 32);       // 球部分分割数を調整
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
	DrawLine::GetInstance()->DrawSphere(sphere);
	// 平面の描画
	DrawLine::GetInstance()->DrawPlane(ground);
	// カプセルの描画
	DrawLine::GetInstance()->DrawCapsule(capsule);
	// OBB を描画
	DrawLine::GetInstance()->DrawOBB(obb);

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



	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================

	// ================================================
	// ここからparticle個々の描画
	// ================================================

	particle->Draw();

	// ================================================
	// ここまでparticle個々の描画
	// ================================================
}
