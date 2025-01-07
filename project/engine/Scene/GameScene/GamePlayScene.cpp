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
	enemy = std::make_unique<Object3d>(this);
	effect = std::make_unique<Object3d>(this);

	/*plane->Initialize();
	axis->Initialize();*/
	skyDome->Initialize();
	ground->Initialize();
	playerBase->Initialize();
	playerWeapon->Initialize();
	enemy->Initialize();
	effect->Initialize();

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
	ModelManager::GetInstance()->LoadModel("enemy.obj");
	enemy->SetModel("enemy.obj");
	ModelManager::GetInstance()->LoadModel("effect.obj");
	effect->SetModel("effect.obj");

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

	enemy->SetScale({ 1.0f, 1.0f, 1.0f });
	enemy->SetRotate({ 0.0f, 3.14f, 0.0f });
	enemy->SetTranslate({ 10.0f, 0.0f, 4.0f });

	/*effect->SetScale({ 1.0f, 1.0f, 1.0f });
	effect->SetRotate({ 0.0f, 3.14f, 0.0f });
	effect->SetTranslate({ 1000.0f, 100.0f, 4.0f });*/

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

	WASD->Initialize("Resources/WASD.png");

	exp->Initialize("Resources/exp.png");
	exp->GetPosition();
	exp->SetPosition({ 0.0f,120.0f });

	expJamp->Initialize("Resources/expJamp.png");
	expJamp->GetPosition();
	expJamp->SetPosition({ 0.0f,220.0f });
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	// クールダウンタイマーの更新
	if (isCollisionCooldown) {
		collisionCooldownTimer += 0.016f; // フレームごとにタイマーを増加
		if (collisionCooldownTimer >= collisionCooldownTime) {
			isCollisionCooldown = false; // クールダウン終了
		}
	}

	// 各3Dオブジェクトの更新
	/*plane->Update();
	axis->Update();*/
	skyDome->Update();
	ground->Update();
	playerBase->Update();
	playerWeapon->Update();
	enemy->Update();
	if (effectActive) {
		effect->Update();
	}

	// プレイヤーの移動処理
	UpdatePlayerMovement();
	// カメラの追従処理
	UpdateCamera();
	// 敵の移動処理
	UpdateEnemyMovement();

	// 衝突処理
	HandleCollisions();

	// エフェクトの表示時間を管理
	if (effectActive) {
		effectTimer += 0.016f; // フレームごとにタイマーを増加
		if (effectTimer > effectDuration) {
			effectActive = false; // 表示時間を超えたらエフェクトを非アクティブ化
		}
	}


	// モンスターボール
	monsterBall->SetPosition(MonsterPosition);
	monsterBall->SetSize({ 100.0f,100.0f });
	monsterBall->Update();

	/*ImGui::Begin("monsterBall");
	ImGui::DragFloat2("transformation", &MonsterPosition.x);
	ImGui::End();*/

	WASD->Update();
	exp->Update();
	expJamp->Update();

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
	enemy->SetCamera(currentCamera_);
	if (effectActive) {
		effect->SetCamera(currentCamera_);
	}

}

void GamePlayScene::BackGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の背景描画
	// ================================================

	//monsterBall->Draw();

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
	enemy->Draw();

	if (effectActive) {
		effect->Draw(); // エフェクトの描画
	}

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

	WASD->Draw();
	exp->Draw();
	expJamp->Draw();

	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}

void GamePlayScene::UpdatePlayerMovement()
{
	Vector3 playerPosition = playerBase->GetTranslate(); // 現在のプレイヤー位置を取得
	Vector3 playerRotation = playerBase->GetRotate();    // 現在のプレイヤー回転を取得
	const float velocity = 0.4f;                         // 移動速度
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

void GamePlayScene::UpdateEnemyMovement()
{
	// 現在の敵の位置を取得
	Vector3 enemyPosition = enemy->GetTranslate();

	// ランダム移動のための値を計算
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(-0.01f, 0.01f);

	// ランダムな速度を適用
	enemyVelocity.x += dis(gen);
	enemyVelocity.z += dis(gen);

	// 敵の移動範囲を制限
	enemyPosition.x += enemyVelocity.x;
	enemyPosition.z += enemyVelocity.z;

	if (enemyPosition.x < enemyMinBounds.x || enemyPosition.x > enemyMaxBounds.x) {
		enemyVelocity.x = -enemyVelocity.x; // 壁で反射
	}
	if (enemyPosition.z < enemyMinBounds.z || enemyPosition.z > enemyMaxBounds.z) {
		enemyVelocity.z = -enemyVelocity.z; // 壁で反射
	}

	// Y軸は固定
	enemyPosition.y = enemyMinBounds.y;

	// 敵の速度を減衰させる（より滑らかにするため）
	/*enemyVelocity.x *= 0.95f;
	enemyVelocity.z *= 0.95f;*/

	// 敵の位置を更新
	enemy->SetTranslate(enemyPosition);
}

bool GamePlayScene::CheckSphereCollision(const Vector3& center1, float radius1, const Vector3& center2, float radius2)
{
	//// 球の中心間距離を計算
	//float distanceSquared = (center1.x - center2.x) * (center1.x - center2.x) +
	//	(center1.y - center2.y) * (center1.y - center2.y) +
	//	(center1.z - center2.z) * (center1.z - center2.z);

	//// 半径の合計の2乗と比較
	//float radiusSum = radius1 + radius2;
	//float radiusSumSquared = radiusSum * radiusSum;
	//return distanceSquared <= radiusSumSquared;

	 // 中心間の距離を計算
	float dx = center1.x - center2.x;
	float dy = center1.y - center2.y;
	float dz = center1.z - center2.z;

	// 距離の平方
	float distanceSquared = dx * dx + dy * dy + dz * dz;

	// 半径の合計の平方
	float radiusSum = radius1 + radius2;
	float radiusSumSquared = radiusSum * radiusSum;

	//// デバッグ表示
	//ImGui::Begin("Collision Debug");
	//ImGui::Text("distanceSquared: %.2f", distanceSquared);
	//ImGui::Text("radiusSumSquared: %.2f", radiusSumSquared);
	//ImGui::End();

	// 衝突判定
	return distanceSquared <= radiusSumSquared;
}

void GamePlayScene::HandleCollisions()
{
	//// プレイヤーの武器と敵の位置・半径を取得
	//Vector3 weaponPosition = playerWeapon->GetTranslate();
	//float weaponRadius = playerWeapon->GetScale().x / 2.0f; // 武器のスケールから半径を推定
	//Vector3 enemyPosition = enemy->GetTranslate();
	//float enemyRadius = enemy->GetScale().x / 2.0f;         // 敵のスケールから半径を推定

	//// 当たり判定
	//if (CheckSphereCollision(weaponPosition, weaponRadius, enemyPosition, enemyRadius)) {
	//	// 衝突時の処理: エフェクトを敵の位置に表示
	//	effect->SetTranslate(enemyPosition); // 敵の位置にエフェクトを配置
	//	effectActive = true;                 // エフェクトをアクティブに
	//	effectTimer = 0.0f;                  // エフェクトタイマーをリセット
	//}

	  // プレイヤーの武器の位置を取得
	Vector3 weaponPosition = playerWeapon->GetTranslate();
	float weaponRadius = 1.0f; // 固定の半径

	// 敵の位置を取得
	Vector3 enemyPosition = enemy->GetTranslate();

	// 必要に応じてオフセットを適用
	// 敵モデルの中心が正しくない場合、オフセットを加える
	float offsetZ = 1.0f; // 必要に応じて調整
	enemyPosition.z += offsetZ;

	float enemyRadius = 1.0f; // 固定の半径

	//// デバッグ表示
	//ImGui::Begin("Collision Debug");
	//ImGui::Text("Weapon Position: X=%.2f, Y=%.2f, Z=%.2f", weaponPosition.x, weaponPosition.y, weaponPosition.z);
	//ImGui::Text("Enemy Position: X=%.2f, Y=%.2f, Z=%.2f", enemyPosition.x, enemyPosition.y, enemyPosition.z);
	////ImGui::Text("distanceSquared: %.2f", (weaponPosition - enemyPosition).LengthSquared());
	//ImGui::Text("radiusSumSquared: %.2f", (weaponRadius + enemyRadius) * (weaponRadius + enemyRadius));
	//ImGui::End();

	// 当たり判定
	if (CheckSphereCollision(weaponPosition, weaponRadius, enemyPosition, enemyRadius)) {
		// 衝突時の処理: 敵の位置にエフェクトを表示
		effect->SetTranslate(enemyPosition); // 敵の位置にエフェクトを配置
		effectActive = true;                 // エフェクトをアクティブに
		effectTimer = 0.0f;                  // エフェクトタイマーをリセット

		// 衝突回数を増やす
		collisionCount++;

		//// デバッグ表示
		//ImGui::Begin("Collision Debug");
		//ImGui::Text("Collision Count: %d", collisionCount);
		//ImGui::End();

		// 最大回数に達したらシーン切り替え
		if (collisionCount >= maxCollisions) {
			SceneManager::GetInstance()->ChangeScene("TITLE");
		}

		// クールダウン開始
		isCollisionCooldown = true;
		collisionCooldownTimer = 0.0f; // タイマーリセット
	}
}
