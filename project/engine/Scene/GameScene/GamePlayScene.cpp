#include "GamePlayScene.h"
#include <Input.h>
#include "SceneManager.h"

void GamePlayScene::Initialize()
{

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

	/*plane->Initialize();
	axis->Initialize();*/
	skyDome->Initialize();
	ground->Initialize();
	playerBase->Initialize();

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

}

void GamePlayScene::Update()
{
	// 各3Dオブジェクトの更新
	/*plane->Update();
	axis->Update();*/
	skyDome->Update();
	ground->Update();
	playerBase->Update();

	// プレイヤーの移動処理
	UpdatePlayerMovement();


	// モンスターボール
	monsterBall->SetPosition(MonsterPosition);
	monsterBall->SetSize({ 100.0f,100.0f });
	monsterBall->Update();

	ImGui::Begin("monsterBall");
	ImGui::DragFloat2("transformation", &MonsterPosition.x);
	ImGui::End();

	

	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	// カメラの更新
	mainCamera_.Update();
	topCamera_.Update();
	diagonalCamera_.Update();

	// 3Dオブジェクトにカメラを設定
	skyDome->SetCamera(currentCamera_);
	ground->SetCamera(currentCamera_);
	playerBase->SetCamera(currentCamera_);

}

void GamePlayScene::BackGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の背景描画
	// ================================================

	monsterBall->Draw();

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

void GamePlayScene::UpdatePlayerMovement()
{
	Vector3 playerPosition = playerBase->GetTranslate(); // 現在の位置を取得
	const float velocity = 0.5f; // 移動速度

	// 移動処理
	if (Input::GetInstance()->TriggerKey(DIK_W)) {
		playerPosition.z += velocity; // 前方に移動
	}
	if (Input::GetInstance()->TriggerKey(DIK_S)) {
		playerPosition.z -= velocity; // 後方に移動
	}
	if (Input::GetInstance()->TriggerKey(DIK_A)) {
		playerPosition.x -= velocity; // 左に移動
	}
	if (Input::GetInstance()->TriggerKey(DIK_D)) {
		playerPosition.x += velocity; // 右に移動
	}

	// ジャンプ処理
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !isJumping) {
		isJumping = true;
		jumpVelocity = jumpPower; // ジャンプ初速度を設定
	}

	if (isJumping) {
		playerPosition.y += jumpVelocity; // Y座標を更新
		jumpVelocity += gravity; // 重力を適用

		// 地面に着地した場合
		if (playerPosition.y <= 0.0f) {
			playerPosition.y = 0.0f; // 地面の高さにリセット
			isJumping = false; // ジャンプ状態を解除
			jumpVelocity = 0.0f; // 垂直速度をリセット
		}
	}

	playerBase->SetTranslate(playerPosition); // 新しい位置を設定
}
