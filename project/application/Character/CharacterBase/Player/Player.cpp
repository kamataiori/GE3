#include "Player.h"

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

}

void Player::Update()
{
	object3d_->Update();
}

void Player::Draw()
{
	object3d_->Draw();
}
