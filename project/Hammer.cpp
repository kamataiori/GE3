#include "Hammer.h"

void Hammer::Initialize()
{
	TextureManager::GetInstance()->LoadTexture("Resources/hammer.png");
	ModelManager::GetInstance()->LoadModel("hammer.obj");
	object3d_->SetModel("hammer.obj");

	// 3Dオブジェクトの初期化
	object3d_->Initialize();
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,0.5f,0.0f };
	object3d_->SetScale(transform_.scale);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetTranslate(transform_.translate);

	// コライダーの初期化
	CollisionTransform_.translate = object3d_->GetTranslate();
	//SetCollider(this);
	SetPosition(object3d_->GetTranslate());  // 3Dモデルの位置にコライダーをセット
	//SetRotation(object3d_->GetRotate());
	//SetScale(object3d_->GetScale());
	sphere.radius = 1.0f;
}

void Hammer::Update()
{
	object3d_->SetScale(transform_.scale);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetTranslate(transform_.translate);
	object3d_->Update();

	// コライダーの位置を transform_ に連動
	CollisionTransform_.translate = transform_.translate;

	// 追加: コライダーの高さ調整 (必要なら変更)
	CollisionTransform_.translate.y += 4.0f; // 例: 1.0f 上にオフセット

	// コライダーの位置を適用
	SetPosition(CollisionTransform_.translate);
	sphere.color = static_cast<int>(Color::WHITE);
}


void Hammer::Draw()
{
	object3d_->Draw();
	//// SphereCollider の描画
	//SphereCollider::Draw();

}

void Hammer::OnCollision()
{
	sphere.color = static_cast<int>(Color::RED);
}
