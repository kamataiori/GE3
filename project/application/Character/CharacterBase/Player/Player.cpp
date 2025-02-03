#include "Player.h"
#include <Input.h>

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
}

void Player::Update()
{
    // 移動処理
    UpdateMovement();

    // Transform を Object3D に適用
    object3d_->SetTranslate(transform.translate);

    object3d_->Update();
    SetPosition(transform.translate);
}


void Player::Draw()
{
	object3d_->Draw();
	// SphereCollider の描画
	SphereCollider::Draw();
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

    // ジャンプ処理
    if (Input::GetInstance()->TriggerKey(DIK_C) && !isJumping) {
        isJumping = true;
        jumpVelocity = jumpPower;
    }

    if (isJumping) {
        transform.translate.y += jumpVelocity;
        jumpVelocity += gravity;

        if (transform.translate.y <= 0.0f) {
            transform.translate.y = 0.0f;
            isJumping = false;
            jumpVelocity = 0.0f;
        }
    }
}
