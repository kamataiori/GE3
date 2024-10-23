#pragma once
#include "BaseScene.h"
#include "Object3d.h"

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
	/// 描画
	/// </summary>
	void Draw() override;

private:

	// 3Dオブジェクトの初期化
	std::unique_ptr<Object3d> plane = nullptr;

	//3Dカメラの初期化
	std::unique_ptr<CameraManager> cameraManager = nullptr;
	std::unique_ptr<Camera> camera1 = std::make_unique<Camera>();

};

