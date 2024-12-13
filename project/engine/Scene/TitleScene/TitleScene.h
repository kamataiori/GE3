#pragma once
#include "BaseScene.h"
#include "Object3d.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

class TitleScene : public BaseScene
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

	// 3Dオブジェクトの初期化
	std::unique_ptr<Object3d> plane = nullptr;

	////3Dカメラの初期化
	std::unique_ptr<CameraManager> cameraManager_ = nullptr;
	//std::unique_ptr<Camera> camera1 = std::make_unique<Camera>();

	/*std::unique_ptr<ParticleManager> particle = std::make_unique<ParticleManager>();
	std::vector<std::unique_ptr<ParticleEmitter>> emitters;*/

};

