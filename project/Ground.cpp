#include "Ground.h"

void Ground::Initialize()
{

	// モデル読み込み
	TextureManager::GetInstance()->LoadTexture("Resources/graund.png");
	ModelManager::GetInstance()->LoadModel("ground.obj");

	object_->SetModel("ground.obj");

	// 3Dオブジェクトの初期化
	object_->Initialize();
}

void Ground::Update()
{
	// 各3Dオブジェクトの更新
	object_->Update();
}

void Ground::BackGroundDraw()
{
}

void Ground::Draw()
{
	object_->Draw();
}

void Ground::ForeGroundDraw()
{
}
