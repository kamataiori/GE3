#include "Hammer.h"

void Hammer::Initialize()
{
	TextureManager::GetInstance()->LoadTexture("Resources/hammer.png");
	ModelManager::GetInstance()->LoadModel("hammer.obj");
	object3d_->SetModel("hammer.obj");

	// 3Dオブジェクトの初期化
	object3d_->Initialize();
	/*object3d_->SetScale(transform_.scale);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetTranslate(transform_.translate);*/
}

void Hammer::Update()
{
	object3d_->SetScale(transform_.scale);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetTranslate(transform_.translate);
	object3d_->Update();
}

void Hammer::Draw()
{
	object3d_->Draw();
}
