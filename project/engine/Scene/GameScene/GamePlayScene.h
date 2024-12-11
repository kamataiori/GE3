#pragma once
#include <Vector2.h>
#include <vector>
#include <memory>
#include "BaseScene.h"
#include "Skydome.h"
#include "Ground.h"

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
	/*std::unique_ptr<CameraManager> cameraManager_ = std::make_unique<CameraManager>();
	std::unique_ptr<Camera> camera1_ = std::make_unique<Camera>();
	Transform cameraTransform;*/

	std::unique_ptr<Skydome> skyDome_ = std::make_unique<Skydome>(this);

	std::unique_ptr<Ground> ground_ = std::make_unique<Ground>(this);
};

