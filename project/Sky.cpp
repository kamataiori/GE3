#include "Sky.h"

void Sky::Initialize()
{
	TextureManager::GetInstance()->LoadTexture("Resources/sky.png");
	ModelManager::GetInstance()->LoadModel("sky.obj");
	object3d_->SetModel("sky.obj");

	// 3Dオブジェクトの初期化
	object3d_->Initialize();
	//transform_.translate.y = -1.0f;
	object3d_->SetEnableLighting(false);
}

void Sky::Update()
{
	object3d_->SetTranslate(transform_.translate);
	object3d_->Update();
}

void Sky::Draw()
{
	object3d_->Draw();
}
