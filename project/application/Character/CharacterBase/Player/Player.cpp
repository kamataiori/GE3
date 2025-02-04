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

    // ジャンプ開始時に移動方向を保存
    if (isJumpingNow && behavior_ != Behavior::kJump) {
        behavior_ = Behavior::kJump;

        // 直前の移動方向を保存（押されたキーを基準に）
        jumpDirection_ = { 0.0f, 0.0f, 0.0f };
        if (Input::GetInstance()->PushKey(DIK_W)) jumpDirection_.z += velocity;
        if (Input::GetInstance()->PushKey(DIK_S)) jumpDirection_.z -= velocity;
        if (Input::GetInstance()->PushKey(DIK_A)) jumpDirection_.x -= velocity;
        if (Input::GetInstance()->PushKey(DIK_D)) jumpDirection_.x += velocity;
    }
    else if (isMoving && behavior_ != Behavior::kJump) {
        behavior_ = Behavior::kRoot;
    }
    else if (behavior_ != Behavior::kJump) {
        behavior_ = Behavior::kFloat;
    }

    switch (behavior_)
    {
    case Behavior::kRoot:
        UpdateMovement();
        break;
    case Behavior::kJump:
        UpdateJump();
        break;
    case Behavior::kFloat:
        UpdateFloatingGimmick();
        break;
    }

    // Transform を Object3D に適用
    object3d_->SetTranslate(transform.translate);

    object3d_->Update();
    SetPosition(transform.translate);

    // Hammer の Transform を取得
    hammerTransform = hammer_->GetTransform();

    // Player の移動分をそのまま Hammer に適用
    Vector3 hammerOffset = { 1.5f, 0.5f, 0.0f }; // Hammer の相対位置（調整可能）
    hammerTransform.translate = transform.translate + hammerOffset;

    // Hammer に Transform を設定
    hammer_->SetTransform(hammerTransform);

    // Hammer の更新
    hammer_->Update();
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
    if (!isJumping) {
        isJumping = true;
        jumpVelocity = jumpPower;
    }

    if (isJumping) {
        transform.translate.y += jumpVelocity;
        jumpVelocity += gravity;

        // ジャンプ中に移動方向に沿って進む
        transform.translate.x += jumpDirection_.x;
        transform.translate.z += jumpDirection_.z;

        if (transform.translate.y <= 0.0f) {
            transform.translate.y = 0.0f;
            isJumping = false;
            jumpVelocity = 0.0f;
            behavior_ = Behavior::kRoot; // 通常状態に戻す
        }
    }
}
