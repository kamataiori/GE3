#include "Player.h"
#include <Input.h>
#include <corecrt_math_defines.h>

void Player::Initialize()
{
	object3d_->Initialize();

	// モデル読み込み
	ModelManager::GetInstance()->LoadModel("uvChecker.gltf");

	object3d_->SetModel("uvChecker.gltf");

	// モデルにSRTを設定
	object3d_->SetScale({ 1.0f, 1.0f, 1.0f });
	object3d_->SetRotate({ 0.0f, 3.14f, 0.0f });
	object3d_->SetTranslate({ -2.0f, 0.0f, 0.0f });

	// コライダーの初期化
	SetCollider(this);
	SetPosition(object3d_->GetTranslate());  // 3Dモデルの位置にコライダーをセット
	sphere.radius = 2.0f;

    InitializeFloatingGimmick();

    // Hammer の初期化
    hammer_->Initialize();
    // Hammer の Transform を取得
    hammerTransform = hammer_->GetTransform();
    //hammer_->SetCamera(object3d_->GetCamera());
    // Hammer に Transform を設定
    /*hammer_->SetTransform(hammerTransform);*/
}

void Player::Update()
{
    bool isMoving =
        Input::GetInstance()->PushKey(DIK_W) ||
        Input::GetInstance()->PushKey(DIK_S) ||
        Input::GetInstance()->PushKey(DIK_A) ||
        Input::GetInstance()->PushKey(DIK_D);

    bool isJumpingNow = Input::GetInstance()->TriggerKey(DIK_C);
    bool isAttackingNow = Input::GetInstance()->TriggerKey(DIK_V);

    // 攻撃開始
    if (isAttackingNow) {
        behavior_ = Behavior::kAttack;
    }

    // ジャンプ開始
    if (isJumpingNow && !isJumping) {
        isJumping = true;
        jumpVelocity = jumpPower;

        // 直前の移動方向を保存
        jumpDirection_ = { 0.0f, 0.0f, 0.0f };
        if (Input::GetInstance()->PushKey(DIK_W)) jumpDirection_.z += 1.0f;
        if (Input::GetInstance()->PushKey(DIK_S)) jumpDirection_.z -= 1.0f;
        if (Input::GetInstance()->PushKey(DIK_A)) jumpDirection_.x -= 1.0f;
        if (Input::GetInstance()->PushKey(DIK_D)) jumpDirection_.x += 1.0f;

        // 正規化
        float length = sqrt(jumpDirection_.x * jumpDirection_.x + jumpDirection_.z * jumpDirection_.z);
        if (length > 0.0f) {
            jumpDirection_.x = (jumpDirection_.x / length) * 0.5f;
            jumpDirection_.z = (jumpDirection_.z / length) * 0.5f;
        }
    }

    // 状態ごとの処理
    if (behavior_ == Behavior::kAttack) {
        UpdateAttack();
    }
    else if (isJumping) {
        UpdateJump();
    }
    else if (isMoving) {
        behavior_ = Behavior::kRoot;
        UpdateMovement();
    }
    else {
        behavior_ = Behavior::kFloat;
        UpdateFloatingGimmick();
    }


    object3d_->SetTranslate(transform.translate);
    object3d_->Update();
    SetPosition(transform.translate);

    // Hammer の Transform を取得し、プレイヤーの位置に追従
    hammerTransform = hammer_->GetTransform();
    Vector3 hammerOffset = { 0.0f, 2.0f, 0.0f };
    hammerTransform.translate = transform.translate + hammerOffset;
    hammer_->SetTransform(hammerTransform);
    hammer_->Update();

    // ImGui で Hammer の Transform を編集可能にする
    ImGui::Begin("Hammer Transform");
    Vector3 hammerPos = hammerTransform.translate;
    if (ImGui::DragFloat3("Position", &hammerPos.x, 0.1f)) {
        hammerTransform.translate = hammerPos;
        hammer_->SetTransform(hammerTransform);
    }
    Vector3 hammerRot = hammerTransform.rotate;
    if (ImGui::DragFloat3("Rotation", &hammerRot.x, 0.1f)) {
        hammerTransform.rotate = hammerRot;
        hammer_->SetTransform(hammerTransform);
    }
    ImGui::End();
}


void Player::Draw()
{
	object3d_->Draw();
	// SphereCollider の描画
	SphereCollider::Draw();

    // Hammer の描画
    hammer_->Draw();
}

void Player::SetCamera(Camera* camera)
{
    object3d_->SetCamera(camera);
    hammer_->SetCamera(camera);
}

void Player::OnCollision()
{
    sphere.color = static_cast<int>(Color::RED);
}

void Player::UpdateMovement()
{
    ImGui::Begin("Player Transform");

    Vector3 position = transform.translate;
    if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
        transform.translate = position;
    }

    Vector3 rotation = object3d_->GetRotate();
    if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f)) {
        object3d_->SetRotate(rotation);
    }

    Vector3 scale = object3d_->GetScale();
    if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.1f, 10.0f)) {
        object3d_->SetScale(scale);
    }

    ImGui::End();

    // キー入力による移動処理
    if (Input::GetInstance()->PushKey(DIK_W)) {
        transform.translate.z += velocity;
    }
    if (Input::GetInstance()->PushKey(DIK_S)) {
        transform.translate.z -= velocity;
    }
    if (Input::GetInstance()->PushKey(DIK_A)) {
        transform.translate.x -= velocity;
    }
    if (Input::GetInstance()->PushKey(DIK_D)) {
        transform.translate.x += velocity;
    }
}

void Player::InitializeFloatingGimmick()
{
    floatingParameter_ = 0.0f;
}

void Player::UpdateFloatingGimmick()
{
    // 浮遊移動のサイクル<frame>
    Period = 120;
    // 1フレームでのパラメータ加算値
    const float Step = 2.0f * (float)M_PI / Period;
    // パラメータを1ステップ分加算
    floatingParameter_ += Step;
    // 2πを超えたら0に戻す
    floatingParameter_ = std::fmod(floatingParameter_, 2.0f * (float)M_PI);
    // 浮遊のふり幅<m>
    const float floatingAmplitude = 0.25f;
    // 浮遊を座標に反映
    transform.translate.y = std::sin(floatingParameter_) * floatingAmplitude;
}

void Player::UpdateJump()
{
    if (isJumping) {
        // 進行方向に沿ってジャンプ中も移動
        transform.translate.x += jumpDirection_.x;
        transform.translate.z += jumpDirection_.z;

        // 重力の影響を適用
        transform.translate.y += jumpVelocity;
        jumpVelocity += gravity;

        // 着地処理
        if (transform.translate.y <= 0.0f) {
            transform.translate.y = 0.0f;
            isJumping = false;
            jumpVelocity = 0.0f;
            behavior_ = Behavior::kRoot;
        }
    }
}

void Player::UpdateAttack()
{
    // Hammer の Transform を取得
    hammerTransform = hammer_->GetTransform();

    static bool increasing = true; // 回転を増やしているかどうかのフラグ

    if (increasing) {
        // 2.0f まで増やす
        hammerTransform.rotate.x += 0.2f;
        if (hammerTransform.rotate.x >= 2.0f) {
            hammerTransform.rotate.x = 2.0f;
            increasing = false; // 戻す方向に変更
        }
    }
    else {
        // 0.0f まで戻す（-= 0.1f で徐々に戻す）
        hammerTransform.rotate.x -= 0.2f;
        if (hammerTransform.rotate.x <= 0.0f) {
            hammerTransform.rotate.x = 0.0f;
            increasing = true; // 次回の攻撃時に再び増やす
            behavior_ = Behavior::kRoot; // 通常状態に戻る
        }
    }

    // Hammer に適用
    hammer_->SetTransform(hammerTransform);
}

