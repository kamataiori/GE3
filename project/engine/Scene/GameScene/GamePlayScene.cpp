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
	playerWeapon = std::make_unique<Object3d>(this);

	/*plane->Initialize();
	axis->Initialize();*/
	skyDome->Initialize();
	ground->Initialize();
	playerBase->Initialize();
	playerWeapon->Initialize();

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
	ModelManager::GetInstance()->LoadModel("playerWeapon.obj");
	playerWeapon->SetModel("playerWeapon.obj");

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

	playerWeapon->SetScale({ 1.5f, 1.0f, 1.0f });
	playerWeapon->SetRotate({ 0.0f, 3.14f, 0.0f });
	playerWeapon->SetTranslate({ 1.0f, 0.0f, 0.0f });

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
	playerWeapon->Update();

	// プレイヤーの移動処理
	UpdatePlayerMovement();
	// カメラの追従処理
	UpdateCamera();


	// モンスターボール
	monsterBall->SetPosition(MonsterPosition);
	monsterBall->SetSize({ 100.0f,100.0f });
	monsterBall->Update();

	ImGui::Begin("monsterBall");
	ImGui::DragFloat2("transformation", &MonsterPosition.x);
	ImGui::End();

	

	if (Input::GetInstance()->TriggerKey(DIK_L)) {
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
	playerWeapon->SetCamera(currentCamera_);

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
	playerWeapon->Draw();

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
	Vector3 playerPosition = playerBase->GetTranslate(); // 現在のプレイヤー位置を取得
	Vector3 playerRotation = playerBase->GetRotate();    // 現在のプレイヤー回転を取得
	const float velocity = 1.0f;                         // 移動速度
	const float rotationSpeed = 0.008f;                   // 回転速度（感度）

	// 攻撃中でない場合のみ移動や回転を許可
	if (!isAttacking) {
		// キー入力による移動処理
		if (Input::GetInstance()->PushKey(DIK_W)) {
			playerPosition.z += velocity; // 前方に移動
		}
		if (Input::GetInstance()->PushKey(DIK_S)) {
			playerPosition.z -= velocity; // 後方に移動
		}
		if (Input::GetInstance()->PushKey(DIK_A)) {
			playerPosition.x -= velocity; // 左に移動
		}
		if (Input::GetInstance()->PushKey(DIK_D)) {
			playerPosition.x += velocity; // 右に移動
		}

		// ジャンプ処理
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !isJumping) {
			isJumping = true;
			jumpVelocity = jumpPower; // ジャンプ初速度を設定
		}

		if (isJumping) {
			playerPosition.y += jumpVelocity; // Y座標を更新
			jumpVelocity += gravity;          // 重力を適用

			// 地面に着地した場合
			if (playerPosition.y <= 0.0f) {
				playerPosition.y = 0.0f; // 地面の高さにリセット
				isJumping = false;      // ジャンプ状態を解除
				jumpVelocity = 0.0f;    // 垂直速度をリセット
			}
		}

		// マウス入力による回転処理
		int deltaX, deltaY;
		Input::GetInstance()->GetMouseDelta(deltaX, deltaY); // マウスの移動量を取得
		playerRotation.y += deltaX * rotationSpeed;         // マウスの横移動でY軸回転を調整
	}

	// 左クリックで攻撃
	if (Input::GetInstance()->TriggerMouseButton(0) && !isAttacking) {
		isAttacking = true;
		attackTimer = 0.0f; // 攻撃状態を開始
	}

	// 攻撃中の武器の回転を設定
	if (isAttacking) {
		attackTimer += 0.016f; // フレーム単位でタイマーを増加
		if (attackTimer < attackDuration) {
			// 攻撃中は武器を前に倒す（X軸を回転）
			playerWeapon->SetRotate({ 90.0f, playerRotation.y, playerRotation.z });
		}
		else {
			// 攻撃状態を終了
			isAttacking = false;
			playerWeapon->SetRotate(playerRotation); // 元の回転に戻す
		}
	}
	else {
		// 通常時の武器の回転
		playerWeapon->SetRotate(playerRotation);
	}

	// プレイヤーと武器の位置を更新
	playerBase->SetTranslate(playerPosition);
	playerBase->SetRotate(playerRotation);

	// 武器の位置をプレイヤーに追従させる
	Vector3 weaponOffset = { 1.5f, 0.0f, 0.0f };          // 武器のオフセット
	Vector3 weaponPosition = playerPosition;            // プレイヤーの位置を基準に計算

	// 武器のオフセットをプレイヤーの回転に合わせて適用
	float cosY = cos(playerRotation.y);
	float sinY = sin(playerRotation.y);
	weaponPosition.x += weaponOffset.x * cosY - weaponOffset.z * sinY;
	weaponPosition.z += weaponOffset.x * sinY + weaponOffset.z * cosY;

	playerWeapon->SetTranslate(weaponPosition);
}





void GamePlayScene::UpdateCamera()
{
	// プレイヤーの位置を取得
	Vector3 playerPosition = playerBase->GetTranslate();

	// カメラの初期位置
	static Vector3 initialCameraOffset = { 0.0f, 0.0f, -30.0f };

	// プレイヤー位置に基づいてカメラを移動
	Vector3 cameraPosition;
	cameraPosition.x = playerPosition.x + initialCameraOffset.x; // 初期オフセットを適用
	cameraPosition.y = playerPosition.y + initialCameraOffset.y; // 高さはオフセット分
	cameraPosition.z = playerPosition.z + initialCameraOffset.z; // プレイヤーの後方に配置

	// カメラの回転角度（プレイヤーの動きに合わせる場合）
	Vector3 cameraRotation = { 0.0f, 0.0f, 0.0f }; // 必要に応じて調整

	// カメラの位置と回転を設定
	mainCamera_.SetTranslate(cameraPosition);
	mainCamera_.SetRotate(cameraRotation);
}
