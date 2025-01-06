#include "GamePlayScene.h"
#include <Input.h>
#include "SceneManager.h"

void GamePlayScene::Initialize()
{
	//-----Spriteの初期化-----

	for (uint32_t i = 0; i < 6; ++i)
	{
		// スプライトを初期化
		auto sprite = std::make_unique<Sprite>();

		if (i % 2 == 0) {
			// 0, 2, 4 は "Resources/uvChecker.png"
			sprite->Initialize("Resources/uvChecker.png");
		}
		else {
			// 1, 3, 5 は "Resources/monsterBall.png"
			sprite->Initialize("Resources/monsterBall.png");
		}

		sprites.push_back(std::move(sprite));
	}

	monsterBall = std::make_unique<Sprite>();
	monsterBall->Initialize("Resources/monsterBall.png");
	MonsterPosition = monsterBall->GetPosition();
	MonsterPosition = { 100.0f,100.0f };

	// 3Dオブジェクトの初期化
	/*plane = std::make_unique<Object3d>(this);
	axis = std::make_unique<Object3d>(this);*/
	skyDome = std::make_unique<Object3d>(this);
	ground = std::make_unique<Object3d>(this);
	playerBase = std::make_unique<Object3d>(this);
	playerHead = std::make_unique<Object3d>(this);
	playerLeft = std::make_unique<Object3d>(this);
	playerRight = std::make_unique<Object3d>(this);

	/*plane->Initialize();
	axis->Initialize();*/
	skyDome->Initialize();
	ground->Initialize();
	playerBase->Initialize();
	playerHead->Initialize();
	playerLeft->Initialize();
	playerRight->Initialize();

	// モデル読み込み
	/*ModelManager::GetInstance()->LoadModel("uvChecker.gltf");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	plane->SetModel("uvChecker.gltf");
	axis->SetModel("axis.obj");*/
	TextureManager::GetInstance()->LoadTexture("Resources/Sky.png");
	ModelManager::GetInstance()->LoadModel("Sky.obj");
	skyDome->SetModel("Sky.obj");
	TextureManager::GetInstance()->LoadTexture("Resources/graund.png");
	ModelManager::GetInstance()->LoadModel("graund.obj");
	ground->SetModel("graund.obj");
	/*TextureManager::GetInstance()->LoadTexture("Resources/player.png");*/
	ModelManager::GetInstance()->LoadModel("playerBase.obj");
	playerBase->SetModel("playerBase.obj");
	ModelManager::GetInstance()->LoadModel("playerHead.obj");
	playerHead->SetModel("playerHead.obj");
	ModelManager::GetInstance()->LoadModel("playerLeft.obj");
	playerLeft->SetModel("playerLeft.obj");
	ModelManager::GetInstance()->LoadModel("playerRight.obj");
	playerRight->SetModel("playerRight.obj");

	// モデルにSRTを設定
	skyDome->SetScale({ 1.0f, 1.0f, 1.0f });
	skyDome->SetRotate({ 0.0f, 3.14f, 0.0f });
	skyDome->SetTranslate({ 0.0f, 0.0f, 0.0f });

	ground->SetScale({ 1.0f, 1.0f, 1.0f });
	ground->SetRotate({ 0.0f, 3.14f, 0.0f });
	ground->SetTranslate({ 0.0f, -2.0f, 0.0f });

	playerBase->SetScale({ 1.0f, 1.0f, 1.0f });
	playerBase->SetRotate({ 0.0f, 3.14f, 0.0f });
	playerBase->SetTranslate({ 0.0f, 0.0f, 0.0f });

	playerHead->SetScale({ 1.0f, 1.0f, 1.0f });
	playerHead->SetRotate({ 0.0f, 3.14f, 0.0f });
	playerHead->SetTranslate({ 0.0f, 1.4f, 0.0f });

	playerLeft->SetScale({ 1.0f, 1.0f, 1.0f });
	playerLeft->SetRotate({ 0.0f, 3.14f, 0.0f });
	playerLeft->SetTranslate({ -0.8f, 1.0f, 0.0f });

	playerRight->SetScale({ 1.0f, 1.0f, 1.0f });
	playerRight->SetRotate({ 0.0f, 3.14f, 0.0f });
	playerRight->SetTranslate({ 0.8f, 1.0f, 0.0f });

	/*plane->SetScale({ 1.0f, 1.0f, 1.0f });
	plane->SetRotate({ 0.0f, 3.14f, 0.0f });
	plane->SetTranslate({ -2.0f, 0.0f, 0.0f });

	axis->SetScale({ 1.0f, 1.0f, 1.0f });
	axis->SetRotate({ 0.0f, 0.0f, 0.0f });
	axis->SetTranslate({ 2.0f, 0.0f, 0.0f });*/

	// カメラの初期化
	mainCamera_.SetTranslate({ 0.0f, 0.0f, -30.0f });
	topCamera_.SetTranslate({ 0.0f, 10.0f, -20.0f });
	diagonalCamera_.SetTranslate({ 5.0f, 5.0f, -20.0f });

	// 現在のカメラを設定
	currentCamera_ = &mainCamera_;

	// Audioの初期化
	audio->Initialize();
	sound = audio->SoundLoadWave("Resources/fanfare.wav");
	//audio->SoundPlayLoopWave(audio->GetXAudio2().Get(), sound);
	//audio->SoundPlayWave(audio->GetXAudio2().Get(), sound);
	isAudio = false;


	// ライト
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


	skyDome->SetEnableLighting(false);
}

void GamePlayScene::Finalize()
{
	// 音声データの解放
	audio->SoundUnload(&sound);

	// Audioの終了処理
	audio->Finalize();
}

void GamePlayScene::Update()
{
	// 各3Dオブジェクトの更新
	/*plane->Update();
	axis->Update();*/
	skyDome->Update();
	ground->Update();
	playerBase->Update();
	playerHead->Update();
	playerLeft->Update();
	playerRight->Update();

	// ImGuiでオブジェクトの情報を表示
	/*plane->ImGuiUpdate("plane");
	axis->ImGuiUpdate("axis");*/

	//// カメラの更新
	//camera1->Update();
	//Vector3 cameraRotate = camera2->GetRotate();
	//cameraRotate.x = 0.0f;
	//cameraRotate.y += 0.1f;
	//cameraRotate.z = 0.0f;
	//camera2->SetRotate(cameraRotate);
	//camera2->Update();

	//// カメラコントロール用のウィンドウを作成
	//ImGui::Begin("Camera Control");

	//// カメラの切り替え
	//if (ImGui::Checkbox("Use Second Camera", &cameraFlag)) {
	//	cameraManager->SetCurrentCamera(cameraFlag ? 1 : 0);
	//}

	//ImGui::End();

	// モンスターボール
	monsterBall->SetPosition(MonsterPosition);
	monsterBall->SetSize({ 100.0f,100.0f });
	monsterBall->Update();

	ImGui::Begin("monsterBall");
	ImGui::DragFloat2("transformation", &MonsterPosition.x);
	ImGui::End();

	// 各スプライトの更新処理
	for (size_t i = 0; i < sprites.size(); ++i) {
		auto& sprite = sprites[i];

		// X座標を設定
		Vector2 position = sprite->GetPosition();
		position.x = initialX + i * offsetX;  // X座標をずらす
		position.y = 100.0f;
		sprite->SetPosition(position);

		// 角度を変化させる
		float rotation = sprite->GetRotation();
		rotation += 0.08f;
		sprite->SetRotation(rotation);

		// 色を変化させる
		Vector4 color = sprite->GetColor();
		color.x += 0.01f;
		if (color.x > 1.0f) {
			color.x -= 1.0f;
		}
		sprite->SetColor(color);

		// スプライトのサイズを設定
		sprite->SetSize(initialSize);
		sprite->SetAnchorPoint({ 0.5f, 0.5f });
		sprite->SetFlipX(false);
		sprite->SetFlipY(false);

		// テクスチャ範囲指定
		if (i % 2 == 0) {
			sprite->SetTextureLeftTop({ 0.0f, 0.0f });
			sprite->SetTextureSize({ 64.0f, 64.0f });
		}
		else {
			sprite->SetTextureLeftTop({ 600.0f, 280.0f });
			sprite->SetTextureSize({ 200.0f, 200.0f });
		}

		// 各スプライトを更新
		sprite->Update();
	}

	/*ImGui::Begin("light");
	ImGui::DragFloat3("transform", &BaseScene::GetLight()->cameraLightData->worldPosition.x, 0.01f);
	ImGui::DragFloat3("DirectionalDirection", &BaseScene::GetLight()->directionalLightData->direction.x, 0.01f);
	ImGui::DragFloat("DirectionalIntensity", &BaseScene::GetLight()->directionalLightData->intensity, 0.01f);
	ImGui::DragFloat3("SpotPosition", &BaseScene::GetLight()->spotLightData->position.x, 0.01f);
	ImGui::DragFloat("SpotIntensity", &BaseScene::GetLight()->spotLightData->intensity, 0.01f);
	ImGui::End();*/

	// 音声再生を無限ループで呼び出す
	//audio->SoundPlayLoopWave(audio->GetXAudio2().Get(), sound);
   /* audio->SoundPlayWave(audio->GetXAudio2().Get(), sound);*/

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//particle->Emit("particle",/*plane->GetTranslate()*/ { 0.0f,0.0f,-4.0f }, 10);
	/*for (auto& emitter : emitters)
	{
		emitter->Update();
	}*/
	//particle->Update();
	// カメラの更新
	mainCamera_.Update();
	topCamera_.Update();
	diagonalCamera_.Update();

	// 3Dオブジェクトにカメラを設定
	/*plane->SetCamera(currentCamera_);
	axis->SetCamera(currentCamera_);*/
	skyDome->SetCamera(currentCamera_);
	ground->SetCamera(currentCamera_);
	playerBase->SetCamera(currentCamera_);
	playerHead->SetCamera(currentCamera_);
	playerLeft->SetCamera(currentCamera_);
	playerRight->SetCamera(currentCamera_);

}

void GamePlayScene::BackGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の背景描画
	// ================================================

	monsterBall->Draw();

	// スプライトを描画する
	for (const auto& sprite : sprites) {
		sprite->Draw();
	}

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

	// 各オブジェクトの描画
	/*plane->Draw();
	axis->Draw();*/
	skyDome->Draw();
	ground->Draw();
	playerBase->Draw();
	playerHead->Draw();
	playerLeft->Draw();
	playerRight->Draw();

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

	//particle->Draw();

	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}
