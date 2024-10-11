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

	// 3Dオブジェクトの初期化
	plane->Initialize();
	axis->Initialize();

	// モデル読み込み
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	plane->SetModel("plane.obj");
	axis->SetModel("axis.obj");

	// モデルにSRTを設定
	plane->SetScale({ 1.0f, 1.0f, 1.0f });
	plane->SetRotate({ 0.0f, 0.0f, 0.0f });
	plane->SetTranslate({ -2.0f, 0.0f, 0.0f });

	axis->SetScale({ 1.0f, 1.0f, 1.0f });
	axis->SetRotate({ 0.0f, 0.0f, 0.0f });
	axis->SetTranslate({ 2.0f, 0.0f, 0.0f });

	// 3Dカメラの初期化
	cameraManager = std::make_unique<CameraManager>();
	camera1->SetTranslate({ 0.0f, 0.0f, -10.0f });
	cameraManager->AddCamera(camera1.get());

	camera2->SetTranslate({ 5.0f, 1.0f, -40.0f });
	cameraManager->AddCamera(camera2.get());

	// カメラのセット
	plane->SetCameraManager(cameraManager.get());
	axis->SetCameraManager(cameraManager.get());

	// Audioの初期化
	audio->Initialize();
	sound = audio->SoundLoadWave("Resources/fanfare.wav");
	audio->SoundPlayLoopWave(audio->GetXAudio2().Get(), sound);
	//audio->SoundPlayWave(audio->GetXAudio2().Get(), sound);
	isAudio = false;
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
	plane->Update();
	axis->Update();

	// ImGuiでオブジェクトの情報を表示
	plane->ImGuiUpdate("plane");
	axis->ImGuiUpdate("axis");

	// カメラの更新
	camera1->Update();
	Vector3 cameraRotate = camera2->GetRotate();
	cameraRotate.x = 0.0f;
	cameraRotate.y += 0.1f;
	cameraRotate.z = 0.0f;
	camera2->SetRotate(cameraRotate);
	camera2->Update();

	// カメラコントロール用のウィンドウを作成
	//ImGui::Begin("Camera Control");

	//// カメラの切り替え
	//if (ImGui::Checkbox("Use Second Camera", &cameraFlag)) {
	//    cameraManager->SetCurrentCamera(cameraFlag ? 1 : 0);
	//}

	//ImGui::End();

	// モンスターボール
	Vector2 MonsterPosition = monsterBall->GetPosition();
	MonsterPosition = { 0.0f,0.0f };
	monsterBall->SetPosition(MonsterPosition);
	monsterBall->SetSize({ 100.0f,100.0f });
	monsterBall->Update();

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

	// 音声再生を無限ループで呼び出す
	//audio->SoundPlayLoopWave(audio->GetXAudio2().Get(), sound);
   /* audio->SoundPlayWave(audio->GetXAudio2().Get(), sound);*/

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
}

void GamePlayScene::Draw()
{


	// 3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
	Object3dCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここから3Dオブジェクト個々の描画
	// ================================================

	// 各オブジェクトの描画
	plane->Draw();
	axis->Draw();

	// ================================================
	// ここまで3Dオブジェクト個々の描画
	// ================================================

	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の描画
	// ================================================
	monsterBall->Draw();

	// スプライトを描画する
	for (const auto& sprite : sprites) {
		sprite->Draw();
	}


	// ================================================
	// ここまでSprite個々の描画
	// ================================================
}
