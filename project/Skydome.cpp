#include "Skydome.h"

void Skydome::Initialize()
{

	// モデル読み込み
	TextureManager::GetInstance()->LoadTexture("Resources/Sky.png");
	ModelManager::GetInstance()->LoadModel("Sky.obj");
	object_->SetModel("Sky.obj");

	// 3Dオブジェクトの初期化
	object_->Initialize();
	object_->SetEnableLighting(false);
}

void Skydome::Update()
{
	// 各3Dオブジェクトの更新
	object_->Update();
	
}

void Skydome::BackGroundDraw()
{
}

void Skydome::Draw()
{
	object_->Draw();
}

void Skydome::ForeGroundDraw()
{
}
