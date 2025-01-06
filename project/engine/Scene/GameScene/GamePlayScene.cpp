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



	skyDome_->Initialize();

	// カメラの初期化
	mainCamera_.SetTranslate({ 0.0f, 0.0f, -20.0f });
	topCamera_.SetTranslate({ 0.0f, 10.0f, -20.0f });
	diagonalCamera_.SetTranslate({ 5.0f, 5.0f, -20.0f });

	// 現在のカメラを設定
	currentCamera_ = &mainCamera_;



	// ライト
	// Lightクラスのデータを初期化
	BaseScene::GetLight()->Initialize();
	BaseScene::GetLight()->GetCameraLight();
	BaseScene::GetLight()->GetDirectionalLight();
	BaseScene::GetLight()->SetDirectionalLightIntensity({ 1.0f });
	BaseScene::GetLight()->SetDirectionalLightColor({ 1.0f,1.0f,1.0f,1.0f });
	
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	// 各3Dオブジェクトの更新
	plane->Update();
	axis->Update();


	// ImGuiでオブジェクトの情報を表示
	plane->ImGuiUpdate("plane");
	axis->ImGuiUpdate("axis");

	

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

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	
	// カメラの更新
	mainCamera_.Update();
	topCamera_.Update();
	diagonalCamera_.Update();

	skyDome_->Update();

	// 3Dオブジェクトにカメラを設定
	plane->InitializeDefaultCamera({ 0.0f, 5.0f, -10.0f }, { 0.0f, 0.0f, 0.0f });
	axis->InitializeDefaultCamera({ 0.0f, 3.0f, -8.0f }, { 0.0f, 45.0f, 0.0f });


	
}

void GamePlayScene::BackGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の背景描画
	// ================================================

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

	//skyDome_->Draw();

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
