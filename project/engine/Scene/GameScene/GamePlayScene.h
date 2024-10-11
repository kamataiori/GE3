#pragma once
#include <Vector2.h>
#include <vector>
#include <memory>
#include <Sprite.h>
#include <Object3d.h>
#include "BaseScene.h"
#include "Audio.h"

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
	/// 描画
	/// </summary>
	void Draw() override;

private:

	float offsetX = 100.0f;  //各スプライトのX座標をずらすオフセット値
	float initialX = 100.0f; //初期X座標

	// Spriteの初期化
	// 小さく描画するための初期スケールを設定
	Vector2 initialSize = { 80.0f,80.0f };
	std::vector<std::unique_ptr<Sprite>> sprites;

	std::unique_ptr<Sprite> monsterBall = std::make_unique<Sprite>();

	// 3Dオブジェクトの初期化
	std::unique_ptr<Object3d> plane = std::make_unique<Object3d>();
	std::unique_ptr<Object3d> axis = std::make_unique<Object3d>();

	//3Dカメラの初期化
	std::unique_ptr<CameraManager> cameraManager = nullptr;
	std::unique_ptr<Camera> camera1 = std::make_unique<Camera>();
	std::unique_ptr<Camera> camera2 = std::make_unique<Camera>();
	bool cameraFlag = false;  //ImGuiで制御するカメラの切り替えフラグ

	std::unique_ptr<Audio> audio = std::make_unique<Audio>();
	Audio::SoundData sound = {};
	bool isAudio = false;
};

