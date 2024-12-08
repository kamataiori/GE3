#pragma once
#include <Vector2.h>
#include <vector>
#include <memory>
#include <Sprite.h>
#include <Object3d.h>
#include "BaseScene.h"
#include "Audio.h"
#include "Light.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

class GamePlayScene : public BaseScene
{
public:
	//------メンバ関数------

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 背景描画
	/// </summary>
	void BackGroundDraw() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 前景描画
	/// </summary>
	void ForeGroundDraw() override;

private:

	//3Dカメラの初期化
	std::unique_ptr<CameraManager> cameraManager = nullptr;
	std::unique_ptr<Camera> camera1 = std::make_unique<Camera>();
	std::unique_ptr<Camera> camera2 = std::make_unique<Camera>();
	bool cameraFlag = false;  //ImGuiで制御するカメラの切り替えフラグ

	std::unique_ptr<ParticleManager> particle = std::make_unique<ParticleManager>();
	std::vector<std::unique_ptr<ParticleEmitter>> emitters;

	std::string textureFilePath = "Resources/particleTest.png"; // テクスチャファイルパス
	ParticleManager::BlendMode blendMode = ParticleManager::BlendMode::kBlendModeAdd; // ブレンドモード

	// カスタムサイズ
	float customSizeX = 32.0f; // 初期サイズX
	float customSizeY = 32.0f; // 初期サイズY

};

