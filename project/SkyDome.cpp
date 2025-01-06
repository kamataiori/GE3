#include "SkyDome.h"

void SkyDome::Initialize()
{
	object_ = std::make_unique<Object3d>();
	camera_ = std::make_unique<Camera>();

	TextureManager::GetInstance()->LoadTexture("Resources/Sky.png");
	ModelManager::GetInstance()->LoadModel("Sky.obj");
	object_->SetModel("Sky.obj");

	// 3Dオブジェクトの初期化
	object_->Initialize();
	object_->SetEnableLighting(false);

	// カメラの初期化
	camera_->SetTranslate({ 0.0f, 0.0f, -10.0f }); // 必要に応じて位置を設定
	camera_->SetRotate({ 0.0f, 0.0f, 0.0f });
	camera_->Update();
}

void SkyDome::Update()
{
	object_->Update();

	// 必要に応じてカメラの更新
	camera_->Update();

}

void SkyDome::BackGroundDraw()
{
}

void SkyDome::Draw()
{
	object_->Draw();
}

void SkyDome::ForeGroundDraw()
{
}
