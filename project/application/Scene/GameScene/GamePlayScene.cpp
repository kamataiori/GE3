#include "GamePlayScene.h"
#include <Input.h>
#include "SceneManager.h"

void GamePlayScene::Initialize()
{
	
	// 3Dカメラの初期化
	camera1->SetTranslate({ 0.0f, 3.5f, -20.0f });

	camera2->SetTranslate({ 5.0f, 1.0f, -40.0f });

	// カメラのセット
	particle->SetCamera(camera1.get());

	player_ = std::make_unique<Player>(this);
	player_->Initialize();
	player_->SetCamera(camera1.get());

	enemy_ = std::make_unique<Enemy>(this);
	enemy_->Initialize();
	enemy_->SetCamera(camera1.get());

	ground_ = std::make_unique<ground>(this);
	ground_->Initialize();
	ground_->SetCamera(camera1.get());

	sky_ = std::make_unique<Sky>(this);
	sky_->Initialize();
	sky_->SetCamera(camera1.get());

	DrawLine::GetInstance()->SetCamera(camera1.get());
	

	// ライト
	// Lightクラスのデータを初期化
	BaseScene::GetLight()->Initialize();
	BaseScene::GetLight()->GetCameraLight();
	BaseScene::GetLight()->GetDirectionalLight();
	BaseScene::GetLight()->SetDirectionalLightIntensity({ 1.0f });
	BaseScene::GetLight()->SetDirectionalLightColor({ 1.0f,1.0f,1.0f,1.0f });
	//BaseScene::GetLight()->SetDirectionalLightDirection(Normalize({ 1.0f,1.0f }));
	/*BaseScene::GetLight()->GetSpotLight();
	BaseScene::GetLight()->SetCameraPosition({ 0.0f, 1.0f, 0.0f });
	BaseScene::GetLight()->SetSpotLightColor({ 1.0f,1.0f,1.0f,1.0f });
	BaseScene::GetLight()->SetSpotLightPosition({ 10.0f,2.25f,0.0f });
	BaseScene::GetLight()->SetSpotLightIntensity({ 4.0f });*/


	particle->Initialize();
	particle->CreateParticleGroup("particle", "Resources/particleTest.png",ParticleManager::BlendMode::kBlendModeAdd);
	//particle->CreateParticleGroup("particle2", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{32.0f,32.0f});
	// ParticleEmitterの初期化
	auto emitter = std::make_unique<ParticleEmitter>(particle.get(), "particle", Transform{ {0.0f, 0.0f, -4.0f} }, 10, 0.5f,true);
	emitters.push_back(std::move(emitter));

	collisionMAnager_ = std::make_unique<CollisionManager>();
	collisionMAnager_->RegisterCollider(player_.get());
	collisionMAnager_->RegisterCollider(enemy_.get());
	collisionMAnager_->RegisterCollider(player_->GetHammer());
}

void GamePlayScene::Finalize()
{
	
}

void GamePlayScene::Update()
{
	ground_->Update();
	player_->Update();
	enemy_->Update();
	sky_->Update();

	// カメラの更新
	camera1->Update();
	Vector3 cameraRotate = camera2->GetRotate();
	cameraRotate.x = 0.0f;
	cameraRotate.y += 0.1f;
	cameraRotate.z = 0.0f;
	camera2->SetRotate(cameraRotate);
	camera2->Update();

	
	
	// 衝突判定と応答
	CheckAllColisions();

	

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//particle->Emit("particle",/*plane->GetTranslate()*/ { 0.0f,0.0f,-4.0f }, 10);
	for (auto& emitter : emitters)
	{
		emitter->Update();
	}
	particle->Update();

	DrawLine::GetInstance()->Update();

	
}

void GamePlayScene::BackGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の背景描画
	// ================================================

	

	// ================================================
	// ここまでSprite個々の背景描画
	// ================================================
}

void GamePlayScene::Draw()
{
	// 3Dオブジェクトの描画前処理。3Dオブジェクトの描画設定に共通のグラフィックスコマンドを積む
	Object3dCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここから3Dオブジェクト個々の描画
	// ================================================

	// 各オブジェクトの描画

	sky_->Draw();
	ground_->Draw();
	player_->Draw();
	enemy_->Draw();
	

	// ================================================
	// ここまで3Dオブジェクト個々の描画
	// ================================================

	// ================================================
	// ここからDrawLine個々の描画
	// ================================================

	

	// ================================================
	// ここまでDrawLine個々の描画
	// ================================================
}

void GamePlayScene::ForeGroundDraw()
{
	// Spriteの描画前処理。Spriteの描画設定に共通のグラフィックスコマンドを積む
	SpriteCommon::GetInstance()->CommonSetting();

	// ================================================
	// ここからSprite個々の前景描画(UIなど)
	// ================================================

	particle->Draw();

	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}

void GamePlayScene::CheckAllColisions()
{
	collisionMAnager_->CheckAllCollisions();
}
