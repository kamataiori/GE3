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
	particle->CreateParticleGroup("particle", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{64.0f,64.0f});
	//particle->CreateParticleGroup("particle2", "Resources/circle.png", ParticleManager::BlendMode::kBlendModeAdd,{32.0f,32.0f});
	// ParticleEmitterの初期化
	auto emitter = std::make_unique<ParticleEmitter>(particle.get(), "particle", Transform{ enemy_->GetPosition() }, 10, 0.5f, true);
	emitters.push_back(std::move(emitter));

	collisionMAnager_ = std::make_unique<CollisionManager>();
	collisionMAnager_->RegisterCollider(enemy_.get());
	collisionMAnager_->RegisterCollider(player_->GetHammer());

	title = std::make_unique<Sprite>();
	title->Initialize("Resources/ex.png");
	title->SetPosition(Vector2(10.0f, 50.0f));

	hp = std::make_unique<Sprite>();
	hp->Initialize("Resources/hp.png");
	// HP スプライトの位置を設定
	hp->SetPosition(Vector2(150.0f, 50.0f));
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

	// HP のスケールを敵の HP に合わせる
	float hpScaleX = static_cast<float>(enemy_->GetHP()); // HP 100 で 1.0, HP 50 で 0.5
	hp->SetSize(Vector2(hpScaleX, 10.0f)); // X スケールを HP に応じて変更
	title->Update();
	hp->Update();

	// 衝突判定と応答
	CheckAllColisions();



	// 敵の HP が 0 ならカウント開始
	if (enemy_->GetHP() <= 0) {
		deathTimer_ += 1.0f / 30.0f; // 1フレームごとに 1/60 秒加算

		// 5秒経過したらシーン切り替え
		if (deathTimer_ >= 5.0f) {
			SceneManager::GetInstance()->ChangeScene("TITLE");
		}
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

	/*title->Draw();
	hp->Draw();*/

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
	if (enemy_->GetHP() > 0) {
		enemy_->Draw();
	}


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
	title->Draw();
	hp->Draw();

	if (enemy_->GetHP() <= 0) {
		particle->Draw();
	}


	// ================================================
	// ここまでSprite個々の前景描画(UIなど)
	// ================================================
}

void GamePlayScene::CheckAllColisions()
{
	collisionMAnager_->CheckAllCollisions();
}
