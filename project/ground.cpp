#include "ground.h"

void ground::Initialize()
{
	TextureManager::GetInstance()->LoadTexture("Resources/ground.png");
	ModelManager::GetInstance()->LoadModel("ground.obj");
	object3d_->SetModel("ground.obj");

	// 3Dオブジェクトの初期化
	object3d_->Initialize();
	transform_.translate.y = -2.0f;
	//object3d_->SetEnableLighting(false);
}

void ground::Update()
{
	object3d_->SetTranslate(transform_.translate);
	object3d_->Update();
}

void ground::Draw()
{
	object3d_->Draw();
}
