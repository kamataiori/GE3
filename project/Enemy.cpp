#include "Enemy.h"

void Enemy::Initialize()
{
	object3d_->Initialize();

	// モデル読み込み
	ModelManager::GetInstance()->LoadModel("uvChecker.gltf");

	object3d_->SetModel("uvChecker.gltf");

	// モデルにSRTを設定
	object3d_->SetScale({ 1.0f, 1.0f, 1.0f });
	object3d_->SetRotate({ 0.0f, 3.14f, 0.0f });
	object3d_->SetTranslate({ 2.0f, 0.0f, 50.0f });

	// コライダーの初期化
	SetCollider(this);
	SetPosition(object3d_->GetTranslate());  // 3Dモデルの位置にコライダーをセット
	//SetRotation(object3d_->GetRotate());
	//SetScale(object3d_->GetScale());
	sphere.radius = 1.5f;

	hp_ = 1000; // HP 初期化
}

void Enemy::Update()
{
	

	object3d_->Update();
	SetPosition(object3d_->GetTranslate());
	//SetScale(object3d_->GetScale());

	// HP が 0 以下なら消滅処理（ここでは非表示にする）
	if (hp_ <= 0) {
		object3d_->SetScale({ 0.0f, 0.0f, 0.0f }); // スケールを 0 にして見えなくする
	}
}

void Enemy::Draw()
{
	object3d_->Draw();
	//// SphereCollider の描画
	//SphereCollider::Draw();
}

void Enemy::OnCollision()
{
	// HP を -10 する
	hp_ -= 10;
	if (hp_ < 0) hp_ = 0; // HP が 0 以下にならないようにする
}
